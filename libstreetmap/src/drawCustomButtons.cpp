/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ezgl/graphics.hpp"
#include "ezgl/application.hpp"


#include "drawCustomButtons.h"

//Connect Zoom buttons to callback functions
void connectZoomButtons(ezgl::application* app) {
    GtkWidget* zoomInButton = GTK_WIDGET(app->get_object("zoomInButton"));
    g_signal_connect(G_OBJECT(zoomInButton), "clicked", G_CALLBACK(press_zoom_in(zoomInButton, app)), nullptr);
    std::cout << "Attempt to connect signals" << std::endl;
    
}