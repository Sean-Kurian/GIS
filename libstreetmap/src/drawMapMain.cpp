//==============================================================================
// File Description: 
//
//==============================================================================

#include "m1.h"
#include "m2.h"
#include "globalData.h"
#include "drawMapHelpers.h"

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

void drawMainCanvas(ezgl::renderer* rend);

//
void draw_map() {
    ezgl::application::settings settings;
    settings.main_ui_resource = "libstreetmap/resources/main.ui";
    settings.window_identifier = "MainWindow"; 
    settings.canvas_identifier = "MainCanvas";  
    ezgl::application application(settings); 
    
    ezgl::point2d maxPt(xFromLon(gData.getMaxLon()), yFromLat(gData.getMaxLat()));
    ezgl::rectangle mapCoords{ {0, 0}, maxPt };
    
    application.add_canvas("MainCanvas", drawMainCanvas, mapCoords);
    
    application.run(nullptr, nullptr, nullptr, nullptr);
}

//
void drawMainCanvas(ezgl::renderer* rend) {
    
}

