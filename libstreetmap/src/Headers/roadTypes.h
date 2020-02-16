#ifndef STREETTYPES_H
#define STREETTYPES_H

//==============================================================================
// File Description: 
//
//==============================================================================

enum roadType {
    highway = 0,
    majorRoad,
    minorRoad,
    trail,
    path,
    TYPECOUNT
};

roadType determineRoadType(const std::string& val);

#endif /* STREETTYPES_H */

