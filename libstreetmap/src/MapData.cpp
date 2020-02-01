//==============================================================================
// File Description:
//
//==============================================================================

#include "MapData.h"
#include "m1.h"

#include <algorithm>
#include <iostream>

//==============================================================================
// Constructors / Destructors
//==============================================================================

MapData::MapData() {
    
}

// Only called on program exit
MapData::~MapData() {
    IDsOfStreetName.clear();
    intersectionsOfStreet.clear();
    segsOfIntersection.clear();
    segsOfStreet.clear();
}

// Clears all structures used to store data. Used to load map without needing destructor
void MapData::clearMapData() {
    IDsOfStreetName.clear();
    intersectionsOfStreet.clear();
    segsOfIntersection.clear();
    segsOfStreet.clear();
}

//==============================================================================
// Initializers
//==============================================================================

// Sizes street vectors to their appropriate size to avoid out of index access
void MapData::allocStreetVecs(const unsigned& numStreets) {
    segsOfStreet.resize(numStreets);
    intersectionsOfStreet.resize(numStreets);
}

// Sizes segment vectors to their appropriate size to avoid out of index access
void MapData::allocSegmmentVecs(const unsigned& numSegments) {
    lengthOfSegment.resize(numSegments);
    travelTimeOfSegment.resize(numSegments);
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
    IDsOfStreetName.insert(std::make_pair(streetName, streetID));
}

// Adds segment to an unordered_set inside a vector indexed to its streetID
void MapData::addSegToStreet(const StreetSegmentIndex& segID, const StreetIndex& streetID) {
    segsOfStreet[streetID].push_back(segID);
}

// Calculates length and travel time of seg and adds to vectors indexed by the segID
void MapData::addLengthAndTravelTimeOfSeg(const InfoStreetSegment& SSData, const unsigned& segID) {
    double dist = 0; 
    unsigned numCurves = SSData.curvePointCount;
    // Get location of the intersections where we start and end
    const LatLon fromInt = getIntersectionPosition(SSData.from);
    const LatLon toInt = getIntersectionPosition(SSData.to);
    // If no curves find distance between the two intersections
    if (numCurves == 0) 
        dist = find_distance_between_two_points(std::make_pair(fromInt, toInt));
    else {
        // Loop over the curve points and sum their distance
        for (unsigned i = 0; i <= numCurves; i++) {
            // From first intersection to first curve point
            if (i == 0) 
                dist += find_distance_between_two_points(std::make_pair
                        (fromInt, getStreetSegmentCurvePoint(i, segID)));
            // From last curve point to last intersection
            else if (i == SSData.curvePointCount)
                dist += find_distance_between_two_points(std::make_pair
                        (getStreetSegmentCurvePoint(numCurves - 1, segID), toInt));
            // From one curve point to another
            else
                dist += find_distance_between_two_points(std::make_pair
                    (getStreetSegmentCurvePoint(i - 1, segID), getStreetSegmentCurvePoint(i, segID)));       
        }
    }
    lengthOfSegment[segID] = dist;
    
    // Calculate travel time
    dist *= 0.001;
    travelTimeOfSegment[segID] = (dist * (1 / SSData.speedLimit) * 3600);
}

// Adds intersectionID to a unordered_set inside a vector indexed to its streetID
void MapData::addIntersectToStreet(const IntersectionIndex& intID, const StreetIndex& streetID) {
    intersectionsOfStreet[streetID].insert(intID);
}

// Adds segment to vector containing all segments inside a vector indexed to its intersectionID
void MapData::addSegToIntersection(const StreetSegmentIndex& segID, const IntersectionIndex& intID) {
    segsOfIntersection[intID].push_back(segID);
}

// Adds node index (0 to numNodes) to map keyed to its OSMID
void MapData::addNodeIndexToOSMID(const unsigned& nodeIndex, const OSMID& nodeID) {
    nodeIndexOfOSMID.insert(std::make_pair(nodeID, nodeIndex));
}

// Adds way index (0 to numWays) to map keyed to its OSMID
void MapData::addWayIndexToOSMID(const unsigned& wayIndex, const OSMID& wayID) {
    wayIndexOfOSMID.insert(std::make_pair(wayID, wayIndex));
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
        const unsigned nameSize = name.size(); // Num chars after removing spaces
        
        // Finds first possible match. Returns end if no match
        auto itr = IDsOfStreetName.lower_bound(name);
        auto end = IDsOfStreetName.end();
        
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

// Returns a vector containing IDs of all segments along a street
const std::vector<int> MapData::getSegsOfStreet(const StreetIndex& streetID) const {
    return segsOfStreet[streetID];
}

// Returns length of a given segment
double MapData::getLengthOfSegment(const StreetSegmentIndex& segID) const {
    return lengthOfSegment[segID];
}

// Returns time needed to travel at speed limit down a given segment
double MapData::getTravelTimeOfSegment(const StreetSegmentIndex& segID) const {
    return travelTimeOfSegment[segID];
}

// Returns vector containing IDs of all intersections along a street
const std::vector<int> MapData::getIntersectionsOfStreet(const StreetIndex& streetID) const {
    std::vector<int> intersections;
    for (const int& intID : intersectionsOfStreet[streetID])
        intersections.push_back(intID);
    return intersections;
}

// Returns vector containing IDs of all segments at a given intersection
const std::vector<int> MapData::getSegsOfIntersection(const IntersectionIndex& intID) const {
    return segsOfIntersection[intID];
}

// Returns node index (0 to numNodes) of the OSMID. Outputs error if none found
unsigned MapData::getNodeIndexOfOSMID(const OSMID& nodeID) const {
    const auto mapItr = nodeIndexOfOSMID.find(nodeID);
    // If no OSMID match is found the itr will point to end
    if (mapItr == nodeIndexOfOSMID.end()) { 
        std::cerr << "No node found with OSMID " << nodeID << "\n";
        return 0;
    }
    // Found a valid OSMID. Return the node index of the OSMID
    else
        return mapItr->second;
}

// Returns way index (0 to numWays) of the OSMID. Outputs error if none found
unsigned MapData::getWayIndexOfOSMID(const OSMID& wayID) const {
    const auto mapItr = wayIndexOfOSMID.find(wayID);
    // If no OSMID match is found the itr will point to end
    if (mapItr == wayIndexOfOSMID.end()) {
        std::cerr << "No node found with OSMID " << wayID << "\n";
        return 0;
    }
    // Found a valid OSMID. Return the way index of the OSMID
    else
        return mapItr->second;
}