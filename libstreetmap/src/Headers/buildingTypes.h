#ifndef BUILDINGTYPES_H
#define BUILDINGTYPES_H

//==============================================================================
// File Description: 
//
//==============================================================================

#include "StreetsDatabaseAPI.h"

enum buildingType {
    school = 0,
    hospital,
    other,
    BF_TYPECOUNT
};

buildingType determineBuildingType(const unsigned& buildingIndex);

#endif /* BUILDINGTYPES_H */

