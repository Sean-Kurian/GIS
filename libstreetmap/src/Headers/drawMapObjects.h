#ifndef DRAWMAPOBJECTS_H
#define DRAWMAPOBJECTS_H

//==============================================================================
// File Description: Functions to draw individual objects on map such as streets, 
// buildings, icons etc. Also functions to draw highlighted data for user
//==============================================================================

#include "StreetsDatabaseAPI.h"
#include "globalData.h"
#include "HighlightedData.h"
#include "roadTypes.h"

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "libcurl.h"

// Draws streets with their width varying based on the number of lanes
void drawStreets(ezgl::renderer* rend, const roadType& type, const double& pixelsPerMeter);
// Draws paths which are things such as trails. Intended for walking not cars
void drawPaths(ezgl::renderer* rend, const double& pixelsPerMeter);
// Draws the names of streets based on the visible size of a segment. Draws one way arrows
void drawStreetNames(ezgl::renderer* rend, const roadType& type, const double& pixelsPerMeter);

// Draws all features besides buildings based on area then draws streams on top
void drawAllFeatures(ezgl::renderer* rend, const double& pixelsPerMeter);
// Draws all features besides buildings and streams
void drawFeatures(ezgl::renderer* rend);
// Draws streams separately since they're always on top
void drawStreams(ezgl::renderer* rend, const double& pixelsPerMeter);

// Draws all buildings regardless of type
void drawAllBuildings(ezgl::renderer* rend);
// Draws all buildings of any given building type
void drawBuildings(ezgl::renderer* rend, const buildingType& type);

// Draws highlights over intersections that need to be highlighted
void drawHighlightedIntersections(ezgl::renderer* rend, const double& pixelsPerMeter);

// Draws school/hospital icons at 30x30, 50x50, or 70x70 pixels
void drawPOI30(ezgl::renderer* rend, const buildingType& type);
void drawPOI50(ezgl::renderer* rend, const buildingType& type);
void drawPOI70(ezgl::renderer* rend, const buildingType& type);

// Draws bus icons at 30x30, 50x50, or 70x70 pixels
void PrintTTCVehicleInfo30(ptree &ptRoot, ezgl::renderer* rend);
void PrintTTCVehicleInfo50(ptree &ptRoot, ezgl::renderer* rend);
void PrintTTCVehicleInfo70(ptree &ptRoot, ezgl::renderer* rend);

#endif /* DRAWMAPOBJECTS_H */

