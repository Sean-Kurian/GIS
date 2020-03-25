//==============================================================================
// File Description: Functions which control what our application does when a 
// users clicks mouse or hits key on keyboard
//==============================================================================

#include "mouseAndKBCtrl.h"
#include "globalData.h"
#include "m1.h"
#include "displayInfo.h"
#include "drawMapHelpers.h"
#include "colourFunctions.h"
#include "LatLon.h"
#include "displayInfo.h"

// Determines what to do based on where the user clicked on the map
void actOnMousePress(ezgl::application* app, GdkEventButton* event, double x, double y) {
    //app->update_message("Mouse Clicked");
    //std::cout << "User clicked mouse at (" << x << "," << y << ")\n";
    LatLon clicked = LatLon(latFromY(y), lonFromX(x));
    
    //If it is a left click, display intersection info
    unsigned intIndex; 
    if (event->button == 1) {
        intIndex = find_closest_intersection(clicked);
        //std::cout << "Closest intersection: " << intIndex << "\n";
        
        //Determine if application is in main mode, if so, display intersection info
        GtkWidget* directionPanel = (GtkWidget*) app->get_object("DirectionPanel");
        if (!gtk_widget_get_visible(directionPanel)) {
            gData.removeLastHighlightedInt(); 
            gData.addHighlightedInt(intIndex);
            displayIntersectionInfo(app, intIndex);
        }
        
        //If the the application is in direction mode, determine if the click is setting starting or destination intersection
        else {
            //Get both search bars in direction mode
            GtkEntry* startingEntry = (GtkEntry*) app->get_object("searchBar");
            GtkEntry* destinationEntry = (GtkEntry*) app->get_object("secondSearchBar");
            
            //If the click is setting the starting intersection
            if (gtk_widget_is_focus((GtkWidget*) startingEntry)) {
                //Determine if a starting intersection is already highlighted, remove if necessary
                if (gData.isStartHighlighted()) {
                    gData.removeLastHighlightedInt();
                }
                gData.addHighlightedInt(intIndex);
                gData.setStartHighlight(true);
                displayIntersectionInfo(app, intIndex);
                
                //Put the name of the intersection clicked on in the start search bar
                std::string intersectionName = getIntersectionName(intIndex);
                gtk_entry_set_text(startingEntry, intersectionName.c_str());
                
                //Toggle to destination entry
                if (!gData.isDestinationHighlighted()) {
                    gtk_entry_set_text(destinationEntry, "Enter Destination Intersection");
                }
                gtk_widget_grab_focus((GtkWidget*) destinationEntry);
            }
            
            //If the click is setting the destination intersection
            else if (gtk_widget_is_focus((GtkWidget*) destinationEntry)) {
                //Determine if a destination intersection is already highlighted, remove if necessary
                if (gData.isDestinationHighlighted()) {
                    gData.removeFirstHighlightedInt();
                }
                gData.addHighlightedIntAtFront(intIndex);
                gData.setDesintationHighlight(true);
                displayIntersectionInfo(app, intIndex);
                
                //Put the name of the intersection clicked on in the destination search bar
                std::string intersectionName = getIntersectionName(intIndex);
                gtk_entry_set_text(destinationEntry, intersectionName.c_str());
                
                //Toggle to Get Directions! button
                GtkWidget* directionsButton = (GtkWidget*) app->get_object("directionRequestButton");
                gtk_widget_grab_focus(directionsButton);
            }
        }
        
        app -> refresh_drawing(); 
        //std::string hi = find_direction_between_intersections(std::make_pair(clicked, clicked));
    }
    
    //If it is not a left click, erase the displayed intersection info if it exists
    else {
        if (gData.getIntersectionInfoBox() != nullptr) {
            eraseIntersectionInfo(gData.getIntersectionInfoBox());
        }
    }
    
   
    
}

// Determines what to do based on which key the user pressed down
void actOnKeyPress(ezgl::application* app, GdkEventKey* event, char* key) {
    if (event->type == GdkEventType::GDK_KEY_PRESS) {
        ezgl::canvas* canvas = app->get_canvas(app->get_main_canvas_id());
        
        // Up, Down, Left, Right pan the map using ezgl functions
        if (strcmp(key, "Up") == 0)
            ezgl::translate_up(canvas, 5.0);
        else if (strcmp(key, "Down") == 0)
            ezgl::translate_down(canvas, 5.0);
        else if (strcmp(key, "Left") == 0)
            ezgl::translate_left(canvas, 5.0);
        else if (strcmp(key, "Right") == 0)
            ezgl::translate_right(canvas, 5.0);
        
        // Alt key force enables nightmode even if its not night time
        else if (event->keyval == GDK_KEY_Alt_L || event->keyval == GDK_KEY_Alt_R) {
            nightMode::isOn = !nightMode::isOn;
            canvas->redraw();
        }
    }
}