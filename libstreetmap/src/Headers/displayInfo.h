//==============================================================================
// File Description: Displays intersection info, popup dialogue boxes and unknown info. 
//
//==============================================================================


#ifndef DISPLAYINFO_H
#define DISPLAYINFO_H

#include "StreetsDatabaseAPI.h"
#include "globalData.h"

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

//Displays intersection info when the intersection is clicked
void displayIntersectionInfo(ezgl::application* app, int intersectionIndex);

//Erases old intersection info when a new intersection is clicked
void eraseIntersectionInfo(GtkWidget* dialog);

//Erases intersection dialog when "x" button is clicked on dialog box
void closeIntersectionInfo(GtkDialog*, gint responseID, gpointer);

//Removes any instances of "<unknown>" from an intersection name
std::string removeUnknown(std::string intersectionName);

//Finds an intersection based on the name
int find_intersection_from_name(std::string intersectionName);

//Alerts user intersection was not found
void intersectionNotFound(ezgl::application* app);

//Alerts user invalid intersection entry
void invalidIntersectionEntry(ezgl::application* app);

//Pops up an error message dialog, pauses program until closed
void popUpErrorMessage(std::string message, ezgl::application* app);

std::string find_direction_between_intersections(std::pair<LatLon, LatLon> points); 

std::string find_turn_direction(std::string dir1, std::string dir2); 

//Displays help screen
void displayHelpScreen(GtkWidget* , ezgl::application* app);

//Prints out driving directions to direction panel
void printDirections(std::vector<std::string> directionsVector, ezgl::application* app);

//Print out walk + driving directions panel
void printWalkDirections(std::vector<std::string> walkingVector, std::vector<std::string> directionsVector, ezgl::application* app);

//Clears the directions label on the direction panel
void clearDirections(ezgl::application* app);

//Combines the vector of strings representing directions into one string for display on GUI
std::string combineDirections(std::vector<std::string> directionsVector, bool isWalking);


#endif /* DISPLAYINFO_H */

