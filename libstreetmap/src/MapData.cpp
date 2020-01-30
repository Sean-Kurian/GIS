#include "MapData.h"
#include "m1.h"

#include <algorithm>

//==============================================================================
// Constructors / Destructors
//==============================================================================
MapData::MapData() {
    
}
MapData::~MapData() {
    IDsOfStreetNames.clear();
    intersectionsOfStreet.clear();
    segsOfIntersection.clear();
}
void MapData::clearMapData() {
    IDsOfStreetNames.clear();
    intersectionsOfStreet.clear();
    segsOfIntersection.clear();
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
        // In-place removal of spaces
        name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());
        // In-place transform to lower case characters
        std::transform(name.begin(), name.end(), name.begin(), 
                       [](unsigned char letter){ return std::tolower(letter); });
        unsigned nameSize = name.size(); // Num chars after removing spaces
        // Finds first possible match. Returns end if no match
        auto itr = IDsOfStreetNames.lower_bound(name);
        auto end = IDsOfStreetNames.end();
        if (itr != end) { // If match exists
            std::string nameMatch = itr->first;
            // Compares strings char by char from position 0 up to name.size())
            while (!nameMatch.compare(0, nameSize, name, 0, nameSize)) {
                // If strings match add the key to vector and try next entry
                streetIDs.push_back(itr->second);
                itr++;
                if (itr != end) // Ensures no dereferencing of end iterator
                    nameMatch = itr->first;
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