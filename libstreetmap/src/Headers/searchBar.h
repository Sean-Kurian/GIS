/* 
 * File:   searchBar.h
 * Author: green107
 * Header file for functions related to the functionality of the search bar
 * Created on February 18, 2020, 10:35 PM
 */

#ifndef SEARCHBAR_H
#define SEARCHBAR_H

#include "StreetsDatabaseAPI.h"
#include "globalData.h"

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

//Connect search bar to callback functions
void connectSearchBar(ezgl::application* app);

//Callback function update search entry content
void updateSearchEntry(GtkSearchEntry* searchEntry, GdkEvent* event);

#endif /* SEARCHBAR_H */

