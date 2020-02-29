/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ezgl/graphics.hpp"
#include "ezgl/application.hpp"
#include "gdk/gdkkeysyms-compat.h"

#include "searchBar.h"
#include "displayInfo.h"
#include "globalData.h"

#include <string>
#include <regex>

//Connect search bar to callback function
void connectSearchBar(ezgl::application* app) {
    GtkSearchEntry* searchBar = (GtkSearchEntry*) app->get_object("searchBar");
    g_signal_connect(G_OBJECT(searchBar), "search-changed", G_CALLBACK(updateSearchEntry), app);
    g_signal_connect(G_OBJECT(searchBar), "activate", G_CALLBACK(searchEnter), app);
}

//Callback function on updated search entry content
void updateSearchEntry(GtkSearchEntry* searchEntry, GdkEvent*) {
    GtkEntry* entry = (GtkEntry*) searchEntry;
    const char* text = gtk_entry_get_text(entry); //For now, this captures the text entered in the search bar
    
}

//Callback function for when the user hits the enter key in the search bar
void searchEnter(GtkEntry* searchEntry, gpointer data) {
    ezgl::application* app = (ezgl::application*) data;
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
            gData.addHighlightedInt(intersectionIndex);
            displayIntersectionInfo(app, intersectionIndex);
            app -> refresh_drawing(); 
            gData.removeLastHighlightedInt(); 
        }
    }
}

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