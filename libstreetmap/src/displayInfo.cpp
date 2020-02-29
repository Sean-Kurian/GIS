#include "ezgl/graphics.hpp"
#include "ezgl/application.hpp"
#include <string>
#include <regex>

#include "displayInfo.h"

//Displays intersection info when the intersection is clicked
void displayIntersectionInfo(ezgl::application* app, int intersectionIndex) {
    //If there was a previous dialog box displaying another intersection, delete it
    //if (gData.getIntersectionInfoBox() != nullptr) {
    //    eraseIntersectionInfo(gData.getIntersectionInfoBox());
    //}
    
    //Get the name of the intersection
    std::string intersectionName = getIntersectionName(intersectionIndex);
    
    //Remove any instances of "<unknown>" from string
    intersectionName = removeUnknown(intersectionName);
    
    //Don't output info if the name is ">" 
    if (intersectionName == ">") {
        return;
    }
    /*//Create the dialog box and display it to the screen
    GObject* window;
    GtkWidget* content_area;
    GtkWidget* label;
    GtkWidget* dialog;
    
    window = app->get_object(app->get_main_window_id().c_str());
    
    dialog = gtk_dialog_new_with_buttons(
            "",
            (GtkWindow*) window,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            NULL, NULL);
    
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    label = gtk_label_new(intersectionName.c_str());
    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show_all(dialog);
    
    //Store the pointer to the dialog box just created
    gData.setIntersectionInfoBox(dialog);
    
    //Connect dialog box to call when "x" button is clicked
    g_signal_connect(G_OBJECT(dialog), "response", G_CALLBACK(closeIntersectionInfo), app);
    */
    
    //TESTING alternative display message in the status bar
    app->update_message(intersectionName);
}

//Erases old intersection info when a new intersection is clicked
void eraseIntersectionInfo(GtkWidget* dialog) {
    gtk_widget_destroy(GTK_WIDGET(dialog));
    gData.setIntersectionInfoBox(nullptr);
}

//Erases intersection dialog when "x" button is clicked on dialog box
void closeIntersectionInfo(GtkDialog*, gint responseID, gpointer) {
    if (responseID == GTK_RESPONSE_DELETE_EVENT) {
        //If there was a previous dialog box displaying another intersection, delete it
        if (gData.getIntersectionInfoBox() != nullptr) {
            eraseIntersectionInfo(gData.getIntersectionInfoBox());
        }
    }
    
}

//Removes any instances of "<unknown>" from an intersection name
std::string removeUnknown(std::string intersectionName) {
    //Declare all the possible <unknown> with & positioning patterns
    std::regex unknownDouble("& <unknown> &");
    std::regex unknownLeft("& <unknown>");
    std::regex unknownRight("<unknown> &");
    std::regex unknown("<unknown");
    
    intersectionName = std::regex_replace(intersectionName, unknownDouble, "");
    intersectionName = std::regex_replace(intersectionName, unknownLeft, "");
    intersectionName = std::regex_replace(intersectionName, unknownRight, "");
    intersectionName = std::regex_replace(intersectionName, unknown, "");
    
    return intersectionName;
}
