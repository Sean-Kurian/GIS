//==============================================================================
// File Description: Helper functions to enable drawing of map objects. 
// For example, Functions to convert lat/lon into our (x,y) or the inverse,
// functions to switch maps, and functions to get angles of street segments
//==============================================================================

#include "m1.h"
#include "globalData.h"
#include "drawMapHelpers.h"
#include "gtkControl.h"

#include <string>
#include <regex>

// Coordinate Functions
// Converts Lon to X
double xFromLon(double lon) {
    double lonRelative = (lon - gData.getAvgLon()) * DEGREE_TO_RADIAN;
    return EARTH_RADIUS_METERS * lonRelative * gData.getLatAspectRatio();
}
// Converts Lat to Y
double yFromLat(double lat) {
    double latRelative = (lat - gData.getAvgLat()) * DEGREE_TO_RADIAN;
    return EARTH_RADIUS_METERS * latRelative;
}
// Converts X to Lon
double lonFromX(double x) {
    return (x / (EARTH_RADIUS_METERS * gData.getLatAspectRatio())) * (1/DEGREE_TO_RADIAN) + gData.getAvgLon(); 
}
// Converts Y to Lat
double latFromY(double y) {
    return ((y / EARTH_RADIUS_METERS) * (1/DEGREE_TO_RADIAN) + (gData.getAvgLat())); 
}

// Gets angle of a street segment or part of a street segment
double getAngle(ezgl::point2d fromPos, ezgl::point2d toPos) {
    if (toPos.x > fromPos.x) 
        return atan2((toPos.y - fromPos.y), (toPos.x - fromPos.x)) * RAD_TO_DEG;
    else 
        return atan2((fromPos.y - toPos.y), (fromPos.x - toPos.x)) * RAD_TO_DEG;
}

// Gets angle of a street segment and returns whether it was flipped
// This ensures street names are not drawn upside down
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

// Adds given number of a given one way arrow to a streets name
std::string addArrows(std::string name, unsigned numArrows, std::string arrow) {
    for (unsigned i = 0; i < numArrows; ++i)
        name = arrow + "  " + name + "  " + arrow;
    return name;
}

// Clipping function to check if a segment is visible to avoid needless calculations
bool segOnScreen(ezgl::renderer* rend, ezgl::point2d fromPos, ezgl::point2d toPos) {
    ezgl::rectangle world = rend->get_visible_world();
    if (world.contains(fromPos) || world.contains(toPos))
        return true;
    return false;
}

// Function to close current map and switch maps to another map
void switchMap(GtkWidget*, gpointer data) {
    // Gets the map drop down to get the map which user wants to switch to
    ezgl::application* app = static_cast<ezgl::application *>(data);
    GtkComboBox* box = (GtkComboBox*) app->get_object("mapDropDown");
    std::string mapName = gtk_combo_box_get_active_id(box);
    
    if (!mapName.empty()) {
        //Display a loading screen before closing map
        GtkWidget* progressBar = createProgressBar();
        GtkWidget* load_screen = displayMapLoadScreen(app, progressBar);
        
        // Closes current map
        close_map();
        increaseProgress(app, progressBar);
        
        // Loads new map
        bool loadSuccess = load_map(mapName);
        if (loadSuccess) {
            increaseProgress(app, progressBar);
            
            // Get the camera to allow us to change the view of the application
            std::string canvasID = app->get_main_canvas_id();
            ezgl::canvas* canvas = app->get_canvas(canvasID);
            ezgl::camera& camera = canvas->get_camera();
            
            ezgl::point2d minPt(xFromLon(gData.getMinLon()), yFromLat(gData.getMinLat()));
            ezgl::point2d maxPt(xFromLon(gData.getMaxLon()), yFromLat(gData.getMaxLat()));
            
            std::cout << "Min point: " << minPt.x << " " << minPt.y << "\n";
            std::cout << "Max point: " << maxPt.x << " " << maxPt.y << "\n";
            ezgl::rectangle newMapCoords{ minPt, maxPt };
            
            // Resets the camera and moves the view to cover our newly loaded map
            camera.reset_world(newMapCoords);
            ezgl::zoom_fit(canvas, camera.get_initial_world());
            
            //Erase loading screen now that the map has been switched
            destroyMapLoadScreen(load_screen);
        }
        else 
            std::cerr << "Error loading new map\n";
    }
}

