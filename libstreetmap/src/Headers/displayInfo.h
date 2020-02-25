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
void closeIntersectionInfo(GtkDialog* dialog, gint responseID, gpointer);


#endif /* DISPLAYINFO_H */

