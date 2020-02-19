/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ezgl/graphics.hpp"
#include "ezgl/application.hpp"
#include "ezgl/callback.hpp"


#include "drawCustomButtons.h"



//Connect Zoom buttons to callback functions
void connectZoomButtons(ezgl::application* app) {
    GtkWidget* zoomInButton = GTK_WIDGET(app->get_object("zoomInButton"));
    g_signal_connect(G_OBJECT(zoomInButton), "clicked", G_CALLBACK(test_call_back), nullptr);
    std::cout << "Attempt to connect signals\n";
    
}

void test_call_back(GtkWidget *widget, ezgl::application *app) {
    std::cout << "Zoom in button pressed\n";
    ezgl::press_zoom_in(widget, app); //This is where the seg fault occurs
}