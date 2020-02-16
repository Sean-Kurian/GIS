#ifndef DRAWMAPOBJECTS_H
#define DRAWMAPOBJECTS_H

//==============================================================================
// File Description: 
//
//==============================================================================

#include "StreetsDatabaseAPI.h"
#include "globalData.h"
#include "roadTypes.h"

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

void drawStreets(ezgl::renderer* rend, const roadType& type, const unsigned& roadWidth);
void drawFeatures(ezgl::renderer* rend);

ezgl::color getFeatureColour(const FeatureType& type);
ezgl::color getRoadColour(const roadType& type);


#endif /* DRAWMAPOBJECTS_H */

