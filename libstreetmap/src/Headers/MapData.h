#ifndef MAPDATA_H
#define MAPDATA_H

#include "StreetsDatabaseAPI.h"
#include <vector>
#include <string>
#include <unordered_set>

class MapData {
private:
    std::vector<std::vector<int> > segsOfIntersection;
    std::vector<std::unordered_set<int> > intersectionsOfStreet;
public:
//==============================================================================
// Constructors / Destructors
//==============================================================================
    MapData();
    ~MapData();
//==============================================================================
// Initializers
//==============================================================================
    void allocStreetVecs(const unsigned& numStreets);
    void allocSegmmentVecs(const unsigned& numSegments);
    void allocIntersectionVecs(const unsigned& numIntersections);
//==============================================================================
// Mutators
//==============================================================================
    void addIntersectToStreet(const IntersectionIndex& intId, const StreetIndex& streetId);
    void addSegToIntersection(const StreetSegmentIndex& segId, const IntersectionIndex& intId);
//==============================================================================
// Accessors
//==============================================================================
    const std::vector<int> getIntersectionsOfStreet(const StreetIndex& streetId) const;  
    const std::vector<int> getSegsOfIntersection(const IntersectionIndex& intId) const;
};

#endif /* MAPDATA_H */

