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

//Callback function for when the user hits the enter key in the search bar
void searchEnter(GtkEntry* searchEntry, gpointer data);

//Callback function for when the user hits the enter key in the destination search bar
//void destinationSearchEnter(GtkEntry* destinationSearch, gpointer data);

//Parses intersection search to format it properly
std::string parseIntersectionSearch(std::string search);

#endif /* SEARCHBAR_H */

