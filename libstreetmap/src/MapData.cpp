#include "MapData.h"
#include "m1.h"

#include <algorithm>
#include <iostream>

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
// Clears all structures used to store data. Used to load map without needing destructor
void MapData::clearMapData() {
    IDsOfStreetNames.clear();
    intersectionsOfStreet.clear();
    segsOfIntersection.clear();
}
//==============================================================================
// Initializers
//==============================================================================
// Sizes street vectors to their appropriate size to avoid out of index access
void MapData::allocStreetVecs(const unsigned& numStreets) {
    intersectionsOfStreet.resize(numStreets);
}
// Sizes segment vectors to their appropriate size to avoid out of index access
void MapData::allocSegmmentVecs(const unsigned& numSegments) {
    
}
// Sizes intersection vectors to their appropriate size to avoid out of index access
void MapData::allocIntersectionVecs(const unsigned& numIntersections) {
    segsOfIntersection.resize(numIntersections);
}
//==============================================================================
// Mutators
//==============================================================================
// Adds streetID to multimap which is keyed to its street name
void MapData::addStreetIDtoName(const StreetIndex& streetID, const std::string& streetName) {
    IDsOfStreetNames.insert(std::make_pair(streetName, streetID));
}
// Adds intersectionID to a unordered_set inside a vector indexed to its streetID
void MapData::addIntersectToStreet(const IntersectionIndex& intID, const StreetIndex& streetID) {
    intersectionsOfStreet[streetID].insert(intID);
}
// Adds segment to vector containing all segments inside a vector indexed to its intersectionID
void MapData::addSegToIntersection(const StreetSegmentIndex& segID, const IntersectionIndex& intID) {
    segsOfIntersection[intID].push_back(segID);
}
//
void MapData::addNodeIndexToOSMID(const unsigned& nodeIndex, const OSMID& nodeID) {
    nodeIndexOfOSMID.insert(std::make_pair(nodeID, nodeIndex));
}
//==============================================================================
// Accessors
//==============================================================================
// Returns vector containing all streetIDs corresponding to the name given
// Works with partial names and ignores spaces (e.g. dund a for Dundas st.)
const std::vector<int> MapData::getStreetIDsFromStreetName(std::string name) const {
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
// Returns vector containing IDs of all intersections along a street
const std::vector<int> MapData::getIntersectionsOfStreet(const StreetIndex& streetID) const {
    std::vector<int> intersections;
    for (int intID : intersectionsOfStreet[streetID])
        intersections.push_back(intID);
    return intersections;
}
// Returns vector containing IDs of all segments at a given intersection
const std::vector<int> MapData::getSegsOfIntersection(const IntersectionIndex& intID) const {
    return segsOfIntersection[intID];
}
//
const unsigned MapData::getNodeIndexOfOSMID(const OSMID& nodeID) const {
    const auto mapItr = nodeIndexOfOSMID.find(nodeID);
    if (mapItr == nodeIndexOfOSMID.end()) {
        std::cerr << "No node found with OSMID " << nodeID << "\n";
        return 0;
    }
    else 
        return mapItr->second;
}
