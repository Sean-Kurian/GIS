#include "MapData.h"
#include "m1.h"

//==============================================================================
// Constructors / Destructors
//==============================================================================
MapData::MapData() {
    
}

MapData::~MapData() {
    
}

//==============================================================================
// Initializers
//==============================================================================
void MapData::allocStreetVecs(const unsigned& numStreets) {
    intersectionsOfStreet.resize(numStreets);
}
void MapData::allocSegmmentVecs(const unsigned& numSegments) {
    
}
void MapData::allocIntersectionVecs(const unsigned& numIntersections) {
    segsOfIntersection.resize(numIntersections);
}
//==============================================================================
// Mutators
//==============================================================================
void MapData::addIntersectToStreet(const IntersectionIndex& intId, const StreetIndex& streetId) {
    intersectionsOfStreet[streetId].insert(intId);
}
void MapData::addSegToIntersection(const StreetSegmentIndex& segId, const IntersectionIndex& intId) {
    segsOfIntersection[intId].push_back(segId);
}
//==============================================================================
// Accessors
//==============================================================================
const std::vector<int> MapData::getIntersectionsOfSteet(const StreetIndex& streetId) const {
    std::vector<int> intersections;
    std::set<int>::iterator start = intersectionsOfStreet[streetId].begin();
    std::set<int>::iterator end = intersectionsOfStreet[streetId].end();
    for (auto itr = start; itr != end; ++itr)
        intersections.push_back(*itr);
    return intersections;
}
const std::vector<int> MapData::getSegsOfIntersection(const IntersectionIndex& intId) const {
    return segsOfIntersection[intId];
}