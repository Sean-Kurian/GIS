#ifndef COLOURFUNCTIONS_H
#define COLOURFUNCTIONS_H

//==============================================================================
// File Description: 
//
//==============================================================================

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

#include "roadTypes.h"
#include "buildingTypes.h"

ezgl::color getFeatureColour(const FeatureType& type);

ezgl::color getRoadColour(const roadType& type);

ezgl::color getBuildingColour(const buildingType& type);

#endif /* COLOURFUNCTIONS_H */

