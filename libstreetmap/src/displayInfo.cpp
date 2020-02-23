/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "ezgl/graphics.hpp"
#include "ezgl/application.hpp"

#include "displayInfo.h"

//Displays intersection info when the intersection is clicked
void displayIntersectionInfo(ezgl::application* app, int intersectionIndex) {
    GObject* window;
    GtkWidget* content_area;
    GtkWidget* label;
    GtkWidget* dialog;
    
    window = app->get_object(app->get_main_window_id().c_str());
    
    dialog = gtk_dialog_new_with_buttons(
            "",
            (GtkWindow*) window,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            NULL);
    
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    label = gtk_label_new("My intersection info goes here");
    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show_all(dialog);
}