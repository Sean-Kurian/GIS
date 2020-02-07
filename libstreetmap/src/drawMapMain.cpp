//==============================================================================
// File Description: 
//
//==============================================================================

#include "m1.h"
#include "m2.h"
#include "globalData.h"

#include "ezgl/application.hpp"

void draw_map() {
    ezgl::application::settings settings;
    settings.main_ui_resource = "main.ui";
    settings.window_identifier = "MainWindow"; 
    settings.canvas_identifier = "MainCanvas";  
    ezgl::application application(settings); 
}