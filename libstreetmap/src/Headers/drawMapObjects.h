#ifndef DRAWMAPOBJECTS_H
#define DRAWMAPOBJECTS_H

//==============================================================================
// File Description: 
//
//==============================================================================

#include "StreetsDatabaseAPI.h"
#include "globalData.h"
#include "HighlightedData.h"
#include "roadTypes.h"

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

void drawStreets(ezgl::renderer* rend, const roadType& type, const double& roadWidth);

void drawStreetNames(ezgl::renderer* rend, const roadType& type, const double& pixelsPerMeter);
void drawOneWayArrows(ezgl::renderer* rend, const roadType& type, const double& pixelsPerMeter);

void drawAllFeatures(ezgl::renderer* rend, const double& pixelsPerMeter);
void drawFeatures(ezgl::renderer* rend);
void drawStreams(ezgl::renderer* rend, const double& pixelsPerMeter);

void drawAllBuildings(ezgl::renderer* rend);
void drawBuildings(ezgl::renderer* rend, const buildingType& type);
void drawPOI(ezgl::renderer* rend); 

void drawHighlightedData(ezgl::renderer* rend);

#endif /* DRAWMAPOBJECTS_H */

