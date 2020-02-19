#ifndef HIGHLIGHTEDDATA_H
#define HIGHLIGHTEDDATA_H

//==============================================================================
// File Description: 
//
//==============================================================================

struct HighlightedData {
    std::vector<unsigned> highlightedInts;
    std::vector<unsigned> highlightedSegs;
    static bool isHighlighted;
};

#endif /* HIGHLIGHTEDDATA_H */

