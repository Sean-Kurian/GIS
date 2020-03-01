#ifndef DRAWMAPOBJECTS_H
#define DRAWMAPOBJECTS_H

//==============================================================================
// File Description: Functions to draw individual objects on map such as streets, buildings, icons etc.
//
//==============================================================================

#include "StreetsDatabaseAPI.h"
#include "globalData.h"
#include "HighlightedData.h"
#include "roadTypes.h"

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "libcurl.h"

void drawStreets(ezgl::renderer* rend, const roadType& type, const double& roadWidth);
void drawPaths(ezgl::renderer* rend, const double& pixelsPerMeter);

void drawStreetNames(ezgl::renderer* rend, const roadType& type, const double& pixelsPerMeter);
void drawOneWayArrows(ezgl::renderer* rend, const roadType& type, const double& pixelsPerMeter);

void drawAllFeatures(ezgl::renderer* rend, const double& pixelsPerMeter);
void drawFeatures(ezgl::renderer* rend);
void drawStreams(ezgl::renderer* rend, const double& pixelsPerMeter);

void drawAllBuildings(ezgl::renderer* rend);
void drawBuildings(ezgl::renderer* rend, const buildingType& type);

void drawPOI30(ezgl::renderer* rend, const buildingType& type, double pixelsPerMeter);
void drawPOI50(ezgl::renderer* rend, const buildingType& type, double pixelsPerMeter);
void drawPOI70(ezgl::renderer* rend, const buildingType& type, double pixelsPerMeter);

void PrintTTCVehicleInfo30(ptree &ptRoot, ezgl::renderer* rend);
void PrintTTCVehicleInfo50(ptree &ptRoot, ezgl::renderer* rend);
void PrintTTCVehicleInfo70(ptree &ptRoot, ezgl::renderer* rend);

void drawHighlightedData(ezgl::renderer* rend);

#endif /* DRAWMAPOBJECTS_H */

