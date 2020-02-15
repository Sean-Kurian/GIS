#ifndef DRAWMAPOBJECTS_H
#define DRAWMAPOBJECTS_H

//==============================================================================
// File Description: 
//
//==============================================================================

#include "StreetsDatabaseAPI.h"

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

void drawStreets(ezgl::renderer* rend);
void drawFeatures(ezgl::renderer* rend);

ezgl::color getColour(FeatureType type);


#endif /* DRAWMAPOBJECTS_H */

