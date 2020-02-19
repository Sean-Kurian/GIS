/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ezgl/graphics.hpp"
#include "ezgl/application.hpp"
#include "gdk/gdkkeysyms-compat.h"

#include "searchBar.h"

//Connect search bar to callback function
void connectSearchBar(ezgl::application* app) {
    GtkSearchEntry* searchBar = (GtkSearchEntry*) app->get_object("searchBar");
    g_signal_connect(G_OBJECT(searchBar), "search-changed", G_CALLBACK(updateSearchEntry), app);
}

//Callback function update search entry content
void updateSearchEntry(GtkSearchEntry* searchEntry, GdkEvent* event) {
    GtkEntry* entry = (GtkEntry*) searchEntry;
    const char* text = gtk_entry_get_text(entry);
    
}