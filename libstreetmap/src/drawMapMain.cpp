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
    rend->set_line_width(10);
    drawAllFeatures(rend);
    
    double pixelsPerMeter = pixelInMeters(rend);

    drawStreets(rend, roadType::minorRoad, pixelsPerMeter);

    drawStreets(rend, roadType::majorRoad, pixelsPerMeter);

    drawStreets(rend, roadType::highway, pixelsPerMeter);
    
    drawBuildings(rend);

    drawHighlightedData(rend);
}

//
double pixelInMeters(ezgl::renderer* rend) {
    ezgl::rectangle world = rend->get_visible_world();
    
    ezgl::rectangle oneMeterBox(world.center(), 1, 1);
    ezgl::rectangle screen1Meter = rend->world_to_screen(oneMeterBox);
    std::cout << "1M box area: " << oneMeterBox.area() << "\n";
    std::cout << "Width, height: " << oneMeterBox.width() << ", " << oneMeterBox.height() << "\n"; 
    std::cout << "Screen1Meter area: " << screen1Meter.area() << "\n";
    std::cout << "Width, height: " << screen1Meter.width() << ", " << screen1Meter.height() << "\n";
    return screen1Meter.width();
}

//Initial setup of the application
void initialSetup(ezgl::application* app, bool newWindow) {
    connectZoomButtons(app);
}

