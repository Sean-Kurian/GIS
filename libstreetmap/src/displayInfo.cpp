//==============================================================================
// File Description: Controls display of intersection info, popup dialogue boxes 
// and unknown info
//==============================================================================

#include "ezgl/graphics.hpp"
#include "ezgl/application.hpp"
#include <string>
#include <regex>

#include "displayInfo.h"
#include "m1.h"

//Displays intersection info when the intersection is clicked
void displayIntersectionInfo(ezgl::application* app, int intersectionIndex) {
    
    //Get the name of the intersection
    std::string intersectionName = getIntersectionName(intersectionIndex);
    
    //Remove any instances of "<unknown>" from string
    intersectionName = removeUnknown(intersectionName);
    
    //Don't output info if the name is ">" 
    if (intersectionName == ">") {
        return;
    }
  
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

//Finds an intersection based on the name
int find_intersection_from_name(std::string intersectionName) {
    int and_text = intersectionName.find("&");
    std::string first_street = intersectionName.substr(0, and_text - 1);
    std::string second_street = intersectionName.substr(and_text + 2);
    
    std::vector<int> first_street_ids = find_street_ids_from_partial_street_name(first_street);
    std::vector<int> second_street_ids = find_street_ids_from_partial_street_name(second_street);
    
    for (int street_id : first_street_ids) {
        for (int second_street_id: second_street_ids) {
            std::vector<int> intersections = find_intersections_of_two_streets(std::make_pair(street_id, second_street_id));
            
            //If there is an intersection, return it
            if (!intersections.empty()) {
                return intersections[0];
            }
        }
    }
    
    //If there was no intersection, return -1
    return -1;
}

//Alerts user intersection was not found
void intersectionNotFound(ezgl::application* app) {
    app->update_message("Intersection Not Found");
}

//Alerts user invalid intersection entry
void invalidIntersectionEntry(ezgl::application* app) {
    popUpErrorMessage("Invalid Intersection entry", app);
}

//Pops up an error message dialog, pauses program until closed
void popUpErrorMessage(std::string message, ezgl::application* app) {
    GtkWindow* window = (GtkWindow*) app->get_object(app->get_main_window_id().c_str());
    
    GtkWidget* dialog = gtk_message_dialog_new(window, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                                "%s", message.c_str());
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

std::string find_direction_between_intersections(std::pair<LatLon, LatLon> points){
    
    double dLon = points.second.lon() - points.first.lon(); 
    
    double y = sin(dLon) * cos(points.second.lat()); 
    double x = cos(points.first.lat()) * sin(points.second.lat()) 
             - sin(points.first.lat()) * cos(points.second.lat()) * cos(dLon); \

    double brng = atan2(y,x); 
    
    brng = brng * 57.29577951;//RADIANS_TO_DEGREES; 
    brng = (int)(brng + 360) % 360; 
    brng = 360 - brng; 
    brng += 90; 
    
    if (brng >= 45 && brng <= 135){
        return "North"; 
    }
    else if (brng >= 135 && brng <= 225){
        return "West"; 
    }
    else if (brng >= 225 && brng <= 315){
        return "South"; 
    }
    else if (brng >= 315 || brng <= 45){
        return "East"; 
    }
}

//Displays help screen
void displayHelpScreen(GtkWidget* , ezgl::application* app) {
    GObject* window;
    GtkWidget* content_area;
    GtkWidget* label;
    GtkWidget* dialog;
    
    //Get main window to attach help screen to
    window = app->get_object(app->get_main_window_id().c_str());
    
    //Create dialog box with an "Ok" button
    dialog = gtk_dialog_new_with_buttons("Instructions", (GtkWindow*) window,
                                        GTK_DIALOG_DESTROY_WITH_PARENT, ("Ok"), 
                                        GTK_RESPONSE_ACCEPT, NULL);
    gtk_widget_set_halign(gtk_dialog_get_action_area(GTK_DIALOG(dialog)), GTK_ALIGN_CENTER);
    
    //Create the label with the instructions for the help screen
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    label = gtk_label_new("Use \"and\" or \"&\" to separate street names when typing in intersections\n\n"
                          "When the direction panel is open, click an intersection on the map to\nautomatically input "
                          "its name into the active search entry bar\n\n"
                          "Use the Alt key to toggle night mode on/off\n\n"
                          "Hold down the scroll wheel or use the arrow keys to pan");
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
    
    //Show the dialog box, pause execution of the application until screen is closed
    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

//Prints out directions to direction panel
void printDirections(std::string directions, ezgl::application* app) {
    GtkLabel* directionsLabel = (GtkLabel*) app->get_object("directionsLabel");
    gtk_label_set_justify(directionsLabel, GTK_JUSTIFY_CENTER);
    gtk_label_set_line_wrap(directionsLabel, true);
    gtk_label_set_max_width_chars(directionsLabel, 10);
    gtk_label_set_text(directionsLabel, directions.c_str());
}

//Clears the directions label on the direction panel
void clearDirections(ezgl::application* app) {
    GtkLabel* directionsLabel = (GtkLabel*) app->get_object("directionsLabel");
    gtk_label_set_text(directionsLabel, "");
}