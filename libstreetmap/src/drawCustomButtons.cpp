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
    GtkWidget* zoomOutButton = GTK_WIDGET(app->get_object("zoomOutButton"));
    
    g_signal_connect(G_OBJECT(zoomInButton), "clicked", G_CALLBACK(ezgl::press_zoom_in), app);
    g_signal_connect(G_OBJECT(zoomOutButton), "clicked", G_CALLBACK(ezgl::press_zoom_out), app);
}

//Set up drop down menu for map switching
void setUpDropDown(ezgl::application* app) {
    GtkComboBox* dropDownMenu = (GtkComboBox*) app->get_object("mapDropDown");
    
    //Set the default selection to be the current map
    std::string mapPath = gData.getMapPath();
    const char* map_path = mapPath.c_str();
    gtk_combo_box_set_active_id(dropDownMenu, map_path);
    
    //Connect object to callback funciton
    g_signal_connect(G_OBJECT(dropDownMenu), "changed", G_CALLBACK(dropDownChanged), app);
}

//Callback function for drop down menu
void dropDownChanged(GtkComboBox* dropDownMenu, gpointer) {
    const char* currentID = gtk_combo_box_get_active_id(dropDownMenu); //For now, this captures the ID of the active entry
}