#ifndef DRAWMAPHELPERS_H
#define DRAWMAPHELPERS_H

//==============================================================================
// File Description: 
//
//==============================================================================

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

#include "gtkObjects.h"

// Coordinate Functions
double xFromLon(double lon);
double yFromLat(double lat);
double lonFromX(double x); 
double latFromY(double y); 

void switchMap(GtkWidget *, gpointer data);

#endif /* DRAWMAPHELPERS_H */

