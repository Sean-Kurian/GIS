//==============================================================================
// File Description: 
//
//==============================================================================

#include "drawMapHelpers.h"
#include "globalData.h"
#include "m1.h"

#include <string>
#include <regex>

// Coordinate Functions
double xFromLon(double lon) {
    double lonRelative = (lon - gData.getAvgLon()) * DEGREE_TO_RADIAN;
    return EARTH_RADIUS_METERS * lonRelative * gData.getLatAspectRatio();
}

double yFromLat(double lat) {
    double latRelative = (lat - gData.getAvgLat()) * DEGREE_TO_RADIAN;
    return EARTH_RADIUS_METERS * latRelative;
}

double lonFromX(double x) {
    return (x / (EARTH_RADIUS_METERS * gData.getLatAspectRatio())) * (1/DEGREE_TO_RADIAN) + gData.getAvgLon(); 
}

double latFromY(double y) {
    return ((y / EARTH_RADIUS_METERS) * (1/DEGREE_TO_RADIAN) + (gData.getAvgLat())); 
}

//
double getAngle(ezgl::point2d fromPos, ezgl::point2d toPos) {
    if (toPos.x > fromPos.x) 
        return atan2((toPos.y - fromPos.y), (toPos.x - fromPos.x)) * RAD_TO_DEG;
    else 
        return atan2((fromPos.y - toPos.y), (fromPos.x - toPos.x)) * RAD_TO_DEG;
}

//
double getAngle(ezgl::point2d fromPos, ezgl::point2d toPos, bool& wasFlipped) {
    if (toPos.x > fromPos.x) {
        wasFlipped = false;
        return atan2((toPos.y - fromPos.y), (toPos.x - fromPos.x)) * RAD_TO_DEG;
    }
    else {
        wasFlipped = true;
        return atan2((fromPos.y - toPos.y), (fromPos.x - toPos.x)) * RAD_TO_DEG;
    }
}

//
std::string addArrows(std::string name, unsigned numArrows, std::string arrow) {
    for (unsigned i = 0; i < numArrows; ++i)
        name = arrow + " " + name + " " + arrow;
    return name;
}

bool segOnScreen(ezgl::renderer* rend, ezgl::point2d fromPos, ezgl::point2d toPos) {
    ezgl::rectangle world = rend->get_visible_world();
    if (world.contains(fromPos) || world.contains(toPos))
        return true;
    else
        return false;
    
}

void switchMap(GtkWidget*, gpointer data) {
    ezgl::application* app = static_cast<ezgl::application *>(data);
    GtkComboBox* box = (GtkComboBox*) app->get_object("mapDropDown");
    std::string mapName = gtk_combo_box_get_active_id(box);
    
    if (!mapName.empty()) {
        //Display a loading screen before closing map
        GtkWidget* progressBar = createProgressBar();
        GtkWidget* load_screen = displayMapLoadScreen(app, progressBar);
        
        close_map();
        
        increaseProgress(app, progressBar);
        
        bool loadSuccess = load_map(mapName);
        if (loadSuccess) {
            increaseProgress(app, progressBar);
            
            std::string canvasID = app->get_main_canvas_id();
            ezgl::canvas* canvas = app->get_canvas(canvasID);
            ezgl::camera& camera = canvas->get_camera();
            
            ezgl::point2d minPt(xFromLon(gData.getMinLon()), yFromLat(gData.getMinLat()));
            ezgl::point2d maxPt(xFromLon(gData.getMaxLon()), yFromLat(gData.getMaxLat()));
            
            std::cout << "Min point: " << minPt.x << " " << minPt.y << "\n";
            std::cout << "Max point: " << maxPt.x << " " << maxPt.y << "\n";
            ezgl::rectangle newMapCoords{ minPt, maxPt };
            
            camera.reset_world(newMapCoords);
            ezgl::zoom_fit(canvas, camera.get_initial_world());
            
            //Erase loading screen now that the map has been switched
            destroyMapLoadScreen(load_screen);
        }
        else 
            std::cerr << "Error loading new map\n";
    }
}

//Displays a loading screen while map switching
GtkWidget* displayMapLoadScreen(ezgl::application* app, GtkWidget* progressBar) {
    //Determine the map being loaded, and create a loading message
    GtkComboBox* dropDownMenu = (GtkComboBox*) app->get_object("mapDropDown");
    const gchar* map_id = gtk_combo_box_get_active_id(dropDownMenu);
    std::string load_message = "Now loading: " + parseMapName(map_id);
    
    //Create the loading screen
    GObject* window;
    GtkWidget* content_area;
    GtkWidget* label;
    GtkWidget* dialog;
    GtkWidget* grid;

    window = app->get_object(app->get_main_window_id().c_str());
    grid = gtk_grid_new();
    dialog = gtk_dialog_new_with_buttons(
            "Loading",
            (GtkWindow*) window,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            NULL, NULL);
    
    //Add the content to the loading screen
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    label = gtk_label_new(load_message.c_str());
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_attach(GTK_GRID(grid), progressBar, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_container_add(GTK_CONTAINER(content_area), grid);
    
    //Display the loading screen
    gtk_widget_show_all(dialog);
    app->refresh_drawing();
    app->flush_drawing();
    
    return dialog;
}

//Destroys the loading screen displayed while map switching
void destroyMapLoadScreen(GtkWidget* dialog) {
    gtk_widget_destroy(dialog);
}

//Parses the map path to return the map name
std::string parseMapName(std::string newMap) {
    const int MAP_NAME_BEGINS = 26;
    const int MAP_NAME_ENDS = newMap.find(".");
    const int NEXT_INDEX = 1;
    newMap = newMap.substr(MAP_NAME_BEGINS, MAP_NAME_ENDS - MAP_NAME_BEGINS);
    
    std::regex dash("-");
    std::regex underscore("_");
    newMap = std::regex_replace(newMap, dash, " ");
    newMap = std::regex_replace(newMap, underscore, ", ");

    newMap[0] = toupper(newMap[0]);
    int capitalIndex = newMap.find(" ") + NEXT_INDEX;
    while (capitalIndex != 0) {
        newMap[capitalIndex] = toupper(newMap[capitalIndex]);
        capitalIndex = newMap.find(" ", capitalIndex) + NEXT_INDEX;
    }
    return newMap;
}

//Creates a progress bar for the load screen
GtkWidget* createProgressBar() {
    const double THIRD = 0.33;
    GtkWidget* progressBar = gtk_progress_bar_new();
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), THIRD);
    return progressBar;
}
//Increase progress bar
void increaseProgress(ezgl::application* app, GtkWidget* progressBar) {
    const double THIRD = 0.33;
    gdouble progress = gtk_progress_bar_get_fraction(GTK_PROGRESS_BAR(progressBar)) + THIRD;
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), progress);
    
    app->flush_drawing();
}
