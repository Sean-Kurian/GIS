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
    water,
    NF_TYPECOUNT
};

naturalFeature determineNaturalFeature(FeatureType type);

#endif /* FEATURETYPES_H */

