/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ezgl/graphics.hpp"
#include "ezgl/application.hpp"
#include "gdk/gdkkeysyms-compat.h"

#include "StreetsDatabaseAPI.h"
#include "drawMapHelpers.h"
#include "searchBar.h"
#include "displayInfo.h"
#include "globalData.h"

#include <string>
#include <regex>

//Connect search bar to callback function
void connectSearchBar(ezgl::application* app) {
    //Main search bar
    GtkSearchEntry* mainSearchBar = (GtkSearchEntry*) app->get_object("mainSearchBar");
    g_signal_connect(G_OBJECT(mainSearchBar), "activate", G_CALLBACK(searchEnter), app);
    
    //Starting search bar
    GtkSearchEntry* searchBar = (GtkSearchEntry*) app->get_object("searchBar");
    g_signal_connect(G_OBJECT(searchBar), "activate", G_CALLBACK(searchEnter), app);
    
    //Destination search bar
    GtkSearchEntry* destinationSearch = (GtkSearchEntry*) app->get_object("secondSearchBar");
    g_signal_connect(G_OBJECT(destinationSearch), "activate", G_CALLBACK(searchEnter), app);
}

//Callback function for when the user hits the enter key in the search bar
void searchEnter(GtkEntry* searchEntry, gpointer data) {
    ezgl::application* app = (ezgl::application*) data;
    auto rend = app->get_renderer();
    std::string search = gtk_entry_get_text(searchEntry);
    
    //If the user is searching for an intersection
    if (search.find("&") != std::string::npos || search.find("and") != std::string::npos) {
        search = parseIntersectionSearch(search);
        int intersectionIndex = find_intersection_from_name(search);
        
        //If the intersection is not found, display that to the user
        if (intersectionIndex == -1) {
            intersectionNotFound(app);
        }
        
        //Otherwise, display intersection info
        else {
                        
            //If using the main search bar, remove the last highlighted intersection and highlight current one
            GtkWidget* mainSearchBar = (GtkWidget*) app->get_object("mainSearchBar");
            if (gtk_widget_get_visible(mainSearchBar)) {
                gData.removeLastHighlightedInt(); 
                gData.addHighlightedInt(intersectionIndex);
            }
            
            //Get the other two search bars in direction mode
            GtkEntry* startingEntry = (GtkEntry*) app->get_object("searchBar");
            GtkEntry* destinationEntry = (GtkEntry*) app->get_object("secondSearchBar");
            
            //If using the destination search bar, remove the current highlighted destination intersection (if it exists)
            //Highlight the current intersection, add it to the front of the highlighted intersections vector
            if (destinationEntry == searchEntry) {
                if (gData.isDestinationHighlighted()) {
                    gData.removeFirstHighlightedInt();
                }
                gData.addHighlightedIntAtFront(intersectionIndex);
                gData.setDesintationHighlight(true);
            }
            
            //If using the starting search bar, remove the current highlighted starting intersection (if it exists)
            //Highlight the current intersection, add it to the back of the highlighted intersections vector
            if (startingEntry == searchEntry) {
                if (gData.isStartHighlighted()) {
                    gData.removeLastHighlightedInt();
                }
                gData.addHighlightedInt(intersectionIndex);
                gData.setStartHighlight(true);
                
                //Toggle to destination entry
                if (!gData.isDestinationHighlighted()) {
                    gtk_entry_set_text(destinationEntry, "Enter Destination Intersection");
                }
                GtkWidget* destinationEntryWidget = (GtkWidget*) destinationEntry;
                gtk_widget_grab_focus(destinationEntryWidget);
            }
            
            //Display intersection info and reposition map
            displayIntersectionInfo(app, intersectionIndex);
            LatLon intPos = getIntersectionPosition(intersectionIndex);
            ezgl::point2d center(xFromLon(intPos.lon()), yFromLat(intPos.lat()));
            ezgl::point2d scale(xFromLon(gData.getMaxLon()) / 30, yFromLat(gData.getMaxLat()) / 30);
            ezgl::rectangle rect(ezgl::point2d(center.x - scale.x, center.y - scale.y),
                                 ezgl::point2d(center.x + scale.x, center.y + scale.y));
            rend->set_visible_world(rect);
            app->refresh_drawing(); 
        }
    }
}

//Callback function for when the user hits the enter key in the destination search bar
/*void destinationSearchEnter(GtkEntry* destinationSearch, gpointer data) {
    ezgl::application* app = (ezgl::application*) data;
    auto rend = app->get_renderer();
    std::string search = gtk_entry_get_text(destinationSearch);
    
    //If the user is searching for an intersection
    if (search.find("&") != std::string::npos || search.find("and") != std::string::npos) {
        search = parseIntersectionSearch(search);
        int intersectionIndex = find_intersection_from_name(search);
        
        //If the intersection is not found, display that to the user
        if (intersectionIndex == -1) {
            intersectionNotFound(app);
        }
        
        //Otherwise, display intersection info
        else {
            gData.addHighlightedInt(intersectionIndex);
            displayIntersectionInfo(app, intersectionIndex);
            LatLon intPos = getIntersectionPosition(intersectionIndex);
            ezgl::point2d center(xFromLon(intPos.lon()), yFromLat(intPos.lat()));
            ezgl::point2d scale(xFromLon(gData.getMaxLon()) / 30, yFromLat(gData.getMaxLat()) / 30);
            ezgl::rectangle rect(ezgl::point2d(center.x - scale.x, center.y - scale.y),
                                 ezgl::point2d(center.x + scale.x, center.y + scale.y));
            rend->set_visible_world(rect);
            app->refresh_drawing(); 
            //gData.removeLastHighlightedInt(); 
        }
    }
}
*/
//Parses intersection search to format it properly
std::string parseIntersectionSearch(std::string search) {
    const int NEXT_INDEX = 1;
    std::regex and_text("and");
    search = std::regex_replace(search, and_text, "&");
    
    search[0] = toupper(search[0]);
    int capitalIndex = search.find(" ") + NEXT_INDEX;
    while (capitalIndex != 0) {
        search[capitalIndex] = toupper(search[capitalIndex]);
        capitalIndex = search.find(" ", capitalIndex) + NEXT_INDEX;
    }
    return search;
}