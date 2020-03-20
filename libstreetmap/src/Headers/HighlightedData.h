#ifndef HIGHLIGHTEDDATA_H
#define HIGHLIGHTEDDATA_H

//==============================================================================
// File Description: Struct which stores vectors containing the segments and the
// intersections which need to be highlighted and a static boolean for whether 
// highlighted data should be drawn at all
//==============================================================================

struct HighlightedData {
    std::vector<unsigned> highlightedInts;
    std::vector<unsigned> highlightedSegs;
    static bool isHighlighted;
};

#endif /* HIGHLIGHTEDDATA_H */

