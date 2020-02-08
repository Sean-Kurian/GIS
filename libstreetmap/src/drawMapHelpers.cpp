//==============================================================================
// File Description: 
//
//==============================================================================

#include "drawMapHelpers.h"
#include "globalData.h"
#include "m1.h"

// Coordinate Functions
double xFromLon(double lon) {
    return EARTH_RADIUS_METERS * (DEGREE_TO_RADIAN * lon) * gData.getLatAspectRatio();
}

double yFromLat(double lat) {
    return EARTH_RADIUS_METERS * (DEGREE_TO_RADIAN * lat);
}