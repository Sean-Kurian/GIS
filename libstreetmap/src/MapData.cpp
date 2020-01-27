#include "MapData.h"
#include "m1.h"

#include <algorithm>

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
void MapData::addStreetIDtoName(const StreetIndex& streetID, const std::string& streetName) {
    IDsOfStreetNames.insert(std::make_pair(streetName, streetID));
}
void MapData::addIntersectToStreet(const IntersectionIndex& intID, const StreetIndex& streetID) {
    intersectionsOfStreet[streetID].insert(intID);
}
void MapData::addSegToIntersection(const StreetSegmentIndex& segID, const IntersectionIndex& intID) {
    segsOfIntersection[intID].push_back(segID);
}
//==============================================================================
// Accessors
//==============================================================================
const std::vector<int> MapData::getStreetIDsFromStreetName(std::string name) {
    std::vector<int> streetIDs;
    if (!name.empty()) {
        auto itr = IDsOfStreetNames.lower_bound(name);
        auto end = IDsOfStreetNames.end();
        if (itr != end) {
            std::string nameMatch = itr->first;
            std::transform(nameMatch.begin(), nameMatch.end(), nameMatch.begin(), ::tolower);
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            
            while (!nameMatch.compare(0, name.size(), name, 0, name.size())) {
                streetIDs.push_back(itr->second);
                itr++;
                if (itr != end) {
                    nameMatch = itr->first;
                    std::transform(nameMatch.begin(), nameMatch.end(), nameMatch.begin(), ::tolower);
                }
                else if (itr == end)
                    return streetIDs;
            }
        }
    }
    return streetIDs;
}
const std::vector<int> MapData::getIntersectionsOfStreet(const StreetIndex& streetID) const {
    std::vector<int> intersections;
    for (int intID : intersectionsOfStreet[streetID])
        intersections.push_back(intID);
    return intersections;
}
const std::vector<int> MapData::getSegsOfIntersection(const IntersectionIndex& intID) const {
    return segsOfIntersection[intID];
}