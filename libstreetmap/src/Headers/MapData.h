#ifndef MAPDATA_H
#define MAPDATA_H

#include "StreetsDatabaseAPI.h"
#include <vector>
#include <string>
#include <set>

class MapData {
private:
    std::vector<std::vector<unsigned> > segsOfIntersection;
    std::vector<std::set<unsigned> > intersectionsOfStreet;
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
//==============================================================================
// Accessors
//==============================================================================
    const std::vector<int> getIntersectionsOfSteet(const StreetIndex& streetId) const;    
};

#endif /* MAPDATA_H */

