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
#include "naturalFeatures.h"

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

void drawStreets(ezgl::renderer* rend, const roadType& type, const double& roadWidth);

void drawStreetNames(ezgl::renderer* rend, const roadType& type, const double& pixelsPerMeter);
void drawOneWayArrows(ezgl::renderer* rend, const roadType& type, const double& pixelsPerMeter);

void drawStreetNames(ezgl::renderer* rend, const roadType& type, const double& pixelsPerMeter, const bool& clearInts);

void drawAllFeatures(ezgl::renderer* rend);
void drawFeatures(ezgl::renderer* rend, const naturalFeature& type);
void drawBuildings(ezgl::renderer* rend);
void drawPOI(ezgl::renderer* rend); 

void drawHighlightedData(ezgl::renderer* rend);

ezgl::color getFeatureColour(const FeatureType& type);
ezgl::color getRoadColour(const roadType& type);


#endif /* DRAWMAPOBJECTS_H */

