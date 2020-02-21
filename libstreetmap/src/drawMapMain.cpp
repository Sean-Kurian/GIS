//==============================================================================
// File Description: 
//
//==============================================================================

#include "m1.h"
#include "m2.h"
#include "StreetsDatabaseAPI.h"
#include "globalData.h"
#include "drawMapObjects.h"
#include "drawMapHelpers.h"
#include "drawCustomButtons.h"
#include "mouseAndKBCtrl.h"
#include "searchBar.h"

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

#include <iostream>

void drawMainCanvas(ezgl::renderer* rend);
double pixelInMeters(ezgl::renderer* rend);
void initialSetup(ezgl::application* app, bool newWindow);

//
void draw_map() {
    ezgl::application::settings settings;
    settings.main_ui_resource = "libstreetmap/resources/main.ui";
    settings.window_identifier = "MainWindow"; 
    settings.canvas_identifier = "MainCanvas";  
    ezgl::application application(settings); 
    
    std::cout << "MinLat / MaxLat: " << gData.getMinLat() << " " << gData.getMaxLat() << "\n";
    std::cout << "MinLon / MaxLon: " << gData.getMinLon() << " " << gData.getMaxLon() << "\n";
    ezgl::point2d minPt(xFromLon(gData.getMinLon()), yFromLat(gData.getMinLat()));
    ezgl::point2d maxPt(xFromLon(gData.getMaxLon()), yFromLat(gData.getMaxLat()));
    std::cout << "Min point: " << minPt.x << " " << minPt.y << "\n";
    std::cout << "Max point: " << maxPt.x << " " << maxPt.y << "\n";
    ezgl::rectangle mapCoords{ minPt, maxPt };
    
    application.add_canvas("MainCanvas", drawMainCanvas, mapCoords);
    
    application.run(initialSetup, actOnMousePress, nullptr, nullptr);
}

//
void drawMainCanvas(ezgl::renderer* rend) {
    rend->set_color(0xE0, 0xE0, 0xE0);
    rend->fill_rectangle(rend->get_visible_world());
    rend->set_line_cap(ezgl::line_cap::round);
    drawAllFeatures(rend);
    
    double pixelsPerMeter = pixelInMeters(rend);
    
    if (pixelsPerMeter > 0.4) {
        drawBuildings(rend);
    }
    
    if (pixelsPerMeter > 0.07) {
        drawStreets(rend, roadType::minorRoad, pixelsPerMeter);
    }
    
    drawStreets(rend, roadType::highway, pixelsPerMeter);
    drawStreets(rend, roadType::majorRoad, pixelsPerMeter);
    
    if (pixelsPerMeter > 0.4) {
        drawStreetNames(rend, roadType::highway, pixelsPerMeter, true);
        drawStreetNames(rend, roadType::majorRoad, pixelsPerMeter, false);
        drawStreetNames(rend, roadType::minorRoad, pixelsPerMeter, false);
        //drawOneWayArrows(rend, roadType::highway, pixelsPerMeter); //Check zoom levels, need to avoid overlap between names and arrows
        //drawOneWayArrows(rend, roadType::majorRoad, pixelsPerMeter);
        //drawOneWayArrows(rend, roadType::minorRoad, pixelsPerMeter);
    }
    
    
    drawHighlightedData(rend);
}

//
double pixelInMeters(ezgl::renderer* rend) {
    ezgl::rectangle world = rend->get_visible_world();
    ezgl::rectangle oneMeterBox(world.center(), 1, 1);
    ezgl::rectangle screen1Meter = rend->world_to_screen(oneMeterBox);
    // std::cout << "1 meter in pixels: " << screen1Meter.width() << "\n";
    return screen1Meter.width();
}

//Initial setup of the application
void initialSetup(ezgl::application* app, bool newWindow) {
    connectZoomButtons(app);
    connectSearchBar(app);
}

