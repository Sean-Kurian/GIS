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

void switchMap(GtkWidget *, gpointer data);

//Displays a loading screen while map switching
GtkWidget* displayMapLoadScreen(ezgl::application* app, GtkWidget* progressBar);

//Destroys the loading screen displayed while map switching
void destroyMapLoadScreen(GtkWidget* dialog);

//Parses the map path to return the map name
std::string parseMapName(std::string newMap);

//Creates a progress bar for the load screen
GtkWidget* createProgressBar();

//Increase progress bar
void increaseProgress(ezgl::application* app, GtkWidget* progress);

#endif /* DRAWMAPHELPERS_H */

