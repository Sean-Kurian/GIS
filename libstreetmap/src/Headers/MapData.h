#ifndef MAPDATA_H
#define MAPDATA_H

#include "StreetsDatabaseAPI.h"
#include <vector>
#include <string>
#include <strings.h>
#include <map>
#include "mapComparitor.cpp"
#include <unordered_set>

class MapData {
private:
    std::vector<std::vector<int> > segsOfIntersection;
    std::vector<std::unordered_set<int> > intersectionsOfStreet;
    std::multimap<unsigned, std::string, mapCaseCompare> IDsOfStreetNames;
    
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

auto comparison = [](const std::string& street1, const std::string& street2) {
        return strcasecmp(street1.c_str(), street2.c_str()) < 0;
};