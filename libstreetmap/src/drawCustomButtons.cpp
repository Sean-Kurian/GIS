#include "ezgl/graphics.hpp"
#include "ezgl/application.hpp"
#include "ezgl/callback.hpp"

#include "drawCustomButtons.h"

namespace gtkObjects {
    GtkComboBox* dropDownMenu;
}

//Connect Zoom buttons to callback functions
void connectZoomButtons(ezgl::application* app) {
    GtkWidget* zoomInButton = GTK_WIDGET(app->get_object("zoomInButton"));
    GtkWidget* zoomOutButton = GTK_WIDGET(app->get_object("zoomOutButton"));
    GtkWidget* zoomFitButton = GTK_WIDGET(app->get_object("zoomFitButton"));
    
    g_signal_connect(G_OBJECT(zoomInButton), "clicked", G_CALLBACK(ezgl::press_zoom_in), app);
    g_signal_connect(G_OBJECT(zoomOutButton), "clicked", G_CALLBACK(ezgl::press_zoom_out), app);
    g_signal_connect(G_OBJECT(zoomFitButton), "clicked", G_CALLBACK(ezgl::press_zoom_fit), app);
}

//Set up drop down menu for map switching
void setUpDropDown(ezgl::application* app) {
    gtkObjects::dropDownMenu = (GtkComboBox*) app->get_object("mapDropDown");
    
    //Set the default selection to be the current map
    std::string mapPath = gData.getMapPath();
    std::replace(mapPath.begin(), mapPath.end(), ' ', '_');
    const char* mapPathCstr = mapPath.c_str();
    gtk_combo_box_set_active_id(gtkObjects::dropDownMenu, mapPathCstr);
    g_signal_connect(G_OBJECT(gtkObjects::dropDownMenu), "changed", G_CALLBACK(switchMap), app);
}
