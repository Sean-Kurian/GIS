#ifndef COORDDATA_H
#define COORDDATA_H

//==============================================================================
// File Description: Struct which stores the minimum, maximum, and average
// latitude and longitude of our currently opened map as well as the cosine
// of the average latitude
//==============================================================================

struct CoordData {
    // Default constructor
    CoordData() : minLat(0), minLon(0), maxLat(0), maxLon(0), 
                  avgLat(0), avgLon(0), latAspectRatio(0) { }
    double minLat, minLon;
    double maxLat, maxLon;
    double avgLat, avgLon;
    double latAspectRatio;
};

#endif /* COORDDATA_H */

