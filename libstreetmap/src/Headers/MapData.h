#ifndef MAPDATA_H
#define MAPDATA_H

#include "StreetsDatabaseAPI.h"
#include <vector>
#include <string>
#include <map>
#include "mapComparitor.cpp"
#include <unordered_set>

class MapData {
private:
    std::multimap<std::string, unsigned, mapCaseCompare> IDsOfStreetNames;
    std::vector<std::unordered_set<int> > intersectionsOfStreet;
    std::vector<std::vector<int> > segsOfIntersection;
    
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
    void addStreetIDtoName(const StreetIndex& streetID, const std::string& streetName);
    void addIntersectToStreet(const IntersectionIndex& intID, const StreetIndex& streetID);
    void addSegToIntersection(const StreetSegmentIndex& segID, const IntersectionIndex& intID);
//==============================================================================
// Accessors
//==============================================================================
    const std::vector<int> getStreetIDsFromStreetName(std::string name);
    const std::vector<int> getIntersectionsOfStreet(const StreetIndex& streetID) const;  
    const std::vector<int> getSegsOfIntersection(const IntersectionIndex& intID) const;
};

#endif /* MAPDATA_H */