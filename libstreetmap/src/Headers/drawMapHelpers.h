#ifndef DRAWMAPHELPERS_H
#define DRAWMAPHELPERS_H

//==============================================================================
// File Description: 
//
//==============================================================================

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

// Coordinate Functions
double xFromLon(double lon);
double yFromLat(double lat);
double lonFromX(double x);
double latFromY(double y);

double getDistFromLine(ezgl::point2d lineStart, ezgl::point2d lineEnd, ezgl::point2d pt);

void switchMap(GtkWidget *, gpointer data);

//Displays a loading screen while map switching
GtkWidget* displayMapLoadScreen(ezgl::application* app);

//Destroys the loading screen displayed while map switching
void destroyMapLoadScreen(GtkWidget* dialog);

//Parses the map path to return the map name
std::string parseMapName(std::string newMap);

#endif /* DRAWMAPHELPERS_H */

