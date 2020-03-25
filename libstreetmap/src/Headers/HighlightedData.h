#ifndef HIGHLIGHTEDDATA_H
#define HIGHLIGHTEDDATA_H

//==============================================================================
// File Description: Struct which stores vectors containing the segments and the
// intersections which need to be highlighted and a static boolean for whether 
// highlighted data should be drawn at all
//==============================================================================

#include "StreetsDatabaseAPI.h"

struct HighlightedData {
    std::vector<IntersectionIndex> highlightedInts;
    std::vector<StreetSegmentIndex> highlightedSegs;
    static bool isHighlighted;
    static bool highlightedOnce; 
    bool startHighlighted;
    bool destinationHighlighted;
};

#endif /* HIGHLIGHTEDDATA_H */

