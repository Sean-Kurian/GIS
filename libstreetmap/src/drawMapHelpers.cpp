//==============================================================================
// File Description: 
//
//==============================================================================

#include "drawMapHelpers.h"
#include "globalData.h"
#include "m1.h"

// Coordinate Functions
double xFromLon(double lon) {
    double lonRelative = (lon - gData.getAvgLon()) * DEGREE_TO_RADIAN;
    return EARTH_RADIUS_METERS * lonRelative * gData.getLatAspectRatio();
}

double yFromLat(double lat) {
    double latRelative = (lat - gData.getAvgLat()) * DEGREE_TO_RADIAN;
    return EARTH_RADIUS_METERS * latRelative;
}

double lonFromX(double x){
    return (x / (EARTH_RADIUS_METERS * gData.getLatAspectRatio())) * (1/DEGREE_TO_RADIAN) + gData.getAvgLon(); 
}

double latFromY(double y){
    return ((y / EARTH_RADIUS_METERS) * (1/DEGREE_TO_RADIAN) + (gData.getAvgLat())); 
}

void switchMap(GtkWidget *, gpointer data) {
    ezgl::application* app = static_cast<ezgl::application *>(data);
    std::string mapName = gtk_combo_box_get_active_id(gtkObjects::dropDownMenu);
    
    if (!mapName.empty()) {
        displayMapLoadScreen(app);
        close_map();
        std::cout << "Loading: " << mapName << "\n";
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
        }
        else 
            std::cerr << "Error loading new map\n";
    }
}

//Displays a loading screen while map switching
void displayMapLoadScreen(ezgl::application* app) {
    //Determine the map being loaded
    GtkComboBox* dropDownMenu = (GtkComboBox*) app->get_object("mapDropDown");
    //GtkTreeIter* iter;
    //gtk_combo_box_get_active_iter(dropDownMenu, iter);
    //GtkTreeModel* model = gtk_combo_box_get_model(dropDownMenu);
    const gchar* map_name = gtk_combo_box_get_active_id(dropDownMenu);
    
    
    //Create the loading screen and display it to the screen
    GObject* window;
    GtkWidget* content_area;
    GtkWidget* label;
    GtkWidget* dialog;
    
    window = app->get_object(app->get_main_window_id().c_str());
    
    dialog = gtk_dialog_new_with_buttons(
            "One moment, please",
            (GtkWindow*) window,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            NULL, NULL);
    
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    label = gtk_label_new(map_name);
    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show_all(dialog);
}

//Destroys the loading screen displayed while map switching
void destoryMapLoadScreen(ezgl::application* app) {
    
}
