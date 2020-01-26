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
const std::vector<int> MapData::getIntersectionsOfStreet(const StreetIndex& streetId) const {
    std::vector<int> intersections;
    for (int intId : intersectionsOfStreet[streetId])
        intersections.push_back(intId);
    return intersections;
}
const std::vector<int> MapData::getSegsOfIntersection(const IntersectionIndex& intId) const {
    return segsOfIntersection[intId];
}