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
    std::cout << "here\n";
    if (!mapName.empty()) {
        close_map();
        std::cout << mapName << "\n";
        bool loadSuccess = load_map(mapName);
        if (loadSuccess) {
            std::string canvasID = app->get_main_canvas_id();
            ezgl::canvas* canvas = app->get_canvas(canvasID);
            ezgl::camera camera = canvas->get_camera();
            
            ezgl::point2d minPt(xFromLon(gData.getMinLon()), yFromLat(gData.getMinLat()));
            ezgl::point2d maxPt(xFromLon(gData.getMaxLon()), yFromLat(gData.getMaxLat()));
            ezgl::rectangle newMapCoords{ minPt, maxPt };
            
            camera.reset_world(newMapCoords);
            app->refresh_drawing();
        }
        else 
            std::cerr << "Error loading new map\n";
    }
}