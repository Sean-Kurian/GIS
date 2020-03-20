#ifndef STREETTYPES_H
#define STREETTYPES_H

//==============================================================================
// File Description: Enumerated type which enables sorting of our roads based on
// their type and a function to determine road type based on OSM data
//==============================================================================

enum roadType {
    highway = 0,
    majorRoad,
    minorRoad,
    trail,
    path,
    RT_TYPECOUNT
};

// Determines the road type of a street segment based on OSM (key, value) data
roadType determineRoadType(const std::string& val);

#endif /* STREETTYPES_H */

