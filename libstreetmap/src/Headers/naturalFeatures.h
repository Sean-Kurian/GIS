#ifndef FEATURETYPES_H
#define FEATURETYPES_H

//==============================================================================
// File Description: 
//
//==============================================================================

#include "StreetsDatabaseAPI.h"

enum naturalFeature {
    forest = 0,
    park,
    island,
    beach,
    lake,
    pond,
    river,
    minorWater,
    NF_TYPECOUNT
};

naturalFeature determineNaturalFeature(const unsigned& featureIndex);

#endif /* FEATURETYPES_H */

