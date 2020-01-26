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
void MapData::allocStreetVectors(const unsigned& numStreets) {
    intersectionsOfStreet.resize(numStreets);
}
//==============================================================================
// Mutators
//==============================================================================
void MapData::addIntersectToStreet(const IntersectionIndex& intId, const StreetIndex& streetId) {
    intersectionsOfStreet[streetId].insert(intId);
}
//==============================================================================
// Accessors
//==============================================================================
const std::vector<int> MapData::getIntersectionsOfSteet(const StreetIndex& streetId) const {
    std::vector<int> intersections;
    std::set<unsigned>::iterator start = intersectionsOfStreet[streetId].begin();
    std::set<unsigned>::iterator end = intersectionsOfStreet[streetId].end();
    for (auto itr = start; itr != end; ++itr)
        intersections.push_back(*itr);
    return intersections;
}