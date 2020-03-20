#ifndef DRAWMAPHELPERS_H
#define DRAWMAPHELPERS_H

//==============================================================================
// File Description: Helper functions to enable drawing of map objects. 
// For example, Functions to convert lat/lon into our (x,y) or the inverse,
// functions to switch maps, and functions to get angles of street segments
//==============================================================================

#include "StreetsDatabaseAPI.h"


#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

constexpr double RAD_TO_DEG = 57.2957795131;

// Coordinate Functions
double xFromLon(double lon);
double yFromLat(double lat);
double lonFromX(double x);
double latFromY(double y);

// Gets angle of a street segment or part of a street segment
double getAngle(ezgl::point2d fromPos, ezgl::point2d toPos);
// Gets angle of a street segment and returns whether it was flipped
// This ensures street names are not drawn upside down
double getAngle(ezgl::point2d fromPos, ezgl::point2d toPos, bool& wasFlipped);

// Adds given number of a given one way arrow to a streets name
std::string addArrows(std::string name, unsigned numArrows, std::string arrow);

// Clipping function to check if a segment is visible to avoid needless calculations
bool segOnScreen(ezgl::renderer* rend, ezgl::point2d fromPos, ezgl::point2d toPos);

// Function to close current map and switch maps to another map
void switchMap(GtkWidget *, gpointer data);

#endif /* DRAWMAPHELPERS_H */

