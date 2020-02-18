//==============================================================================
// File Description: 
//
//==============================================================================

#include "drawMapHelpers.h"
#include "globalData.h"
#include "m1.h"

// Coordinate Functions
double xFromLon(double lon) {
    double lonRelative = (lon - gData.getAvgLon()) * DEGREE_TO_RADIAN;
    return EARTH_RADIUS_METERS * lonRelative * gData.getLatAspectRatio();
}

double yFromLat(double lat) {
    double latRelative = (lat - gData.getAvgLat()) * DEGREE_TO_RADIAN;
    return EARTH_RADIUS_METERS * latRelative;
}

double lonFromX(double x){
    return (x / (EARTH_RADIUS_METERS * gData.getLatAspectRatio())) + (gData.getAvgLon() * (DEGREE_TO_RADIAN)); 
}

double latFromY(double y){
    return ((y / EARTH_RADIUS_METERS) + (gData.getAvgLat() * (DEGREE_TO_RADIAN))); 
}