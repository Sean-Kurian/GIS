#ifndef BUILDINGTYPES_H
#define BUILDINGTYPES_H

//==============================================================================
// File Description: Enumerated type which enables sorting of buildings based on
// their type and a function to determine type based on OSM data
//==============================================================================

#include "StreetsDatabaseAPI.h"

enum buildingType {
    school = 0,
    hospital,
    other,
    BF_TYPECOUNT
};

// Determines the building type of a building based on OSM (key, value) pairs
buildingType determineBuildingType(const unsigned& buildingIndex);

#endif /* BUILDINGTYPES_H */

