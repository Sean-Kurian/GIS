#ifndef BUILDINGTYPES_H
#define BUILDINGTYPES_H

//==============================================================================
// File Description: 
//
//==============================================================================

#include "StreetsDatabaseAPI.h"

enum buildingType {
    residental = 0,
    commercial,
    office,
    school,
    hospital,
    misc,
    BF_TYPECOUNT
};

buildingType determineBuildingType(const unsigned& buildingIndex);

#endif /* BUILDINGTYPES_H */

