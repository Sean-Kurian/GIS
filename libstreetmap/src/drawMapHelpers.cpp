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

double getDistFromLine(ezgl::point2d lineStart, ezgl::point2d lineEnd, ezgl::point2d pt) {
    // Calculates line in form Ax + By = C
    double A = lineStart.y - lineEnd.y;
    double B = lineEnd.x - lineStart.x;
    double C = lineStart.x * lineEnd.y - lineEnd.x * lineStart.y;
    // Calculates distance of the point to the line
    return abs(A * pt.x + B * pt.y + C) / sqrt((A * A) + (B * B));
}

void switchMap(GtkWidget*, gpointer data) {
    ezgl::application* app = static_cast<ezgl::application *>(data);
    GtkComboBox* box = (GtkComboBox*) app->get_object("mapDropDown");
    std::string mapName = gtk_combo_box_get_active_id(box);
    
    if (!mapName.empty()) {
        //Display a loading screen before closing map
        GtkWidget* load_screen = displayMapLoadScreen(app);
        
        close_map();
        
        bool loadSuccess = load_map(mapName);
        if (loadSuccess) {
            
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
GtkWidget* displayMapLoadScreen(ezgl::application* app) {
    //Determine the map being loaded, and create a loading message
    GtkComboBox* dropDownMenu = (GtkComboBox*) app->get_object("mapDropDown");
    const gchar* map_id = gtk_combo_box_get_active_id(dropDownMenu);
    std::string load_message = "Now loading: " + parseMapName(map_id);
    
    //Create the loading screen
    GObject* window;
    GtkWidget* content_area;
    GtkWidget* label;
    GtkWidget* dialog;
    window = app->get_object(app->get_main_window_id().c_str());
    dialog = gtk_dialog_new_with_buttons(
            "Loading",
            (GtkWindow*) window,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            NULL, NULL);
    
    //Add the content to the loading screen
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    label = gtk_label_new(load_message.c_str());
    gtk_container_add(GTK_CONTAINER(content_area), label);
    
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
