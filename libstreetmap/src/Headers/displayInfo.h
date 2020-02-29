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


#endif /* DISPLAYINFO_H */

