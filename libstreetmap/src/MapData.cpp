//==============================================================================
// File Description: Function definitions for class of the global object gData
// Only mutators can change data and they should only be used in load_map
//==============================================================================

#include "MapData.h"
#include "m1.h"
#include "HighlightedData.h"

#include <algorithm>
#include <iostream>
#include <cmath>

//==============================================================================
// Constructors / Destructors
//==============================================================================

MapData::MapData() {
    intersectionDialog = nullptr;
}

// Only called on program exit
MapData::~MapData() {
    IDsOfStreetName.clear();
    segsOfStreet.clear();
    intersectionsOfStreet.clear();
    lengthOfSegment.clear();
    travelTimeOfSegment.clear();
    segsOfIntersection.clear();
    adjacentSegIntsOfInt.clear();
    hlData.highlightedInts.clear(); 
    hlData.highlightedSegs.clear();
    indexesOfStreams.clear();
    areaOfFeatures.clear();
    indexesOfBuildingTypes.clear();
    segsOfStreetType.clear();
    segsOfWayOSMID.clear();
    nodeIndexOfOSMID.clear();
    wayIndexOfOSMID.clear();
    relationIndexOfOSMID.clear();
}

// Clears all data structures. Used to load another map without needing destructor
void MapData::clearMapData() {
    IDsOfStreetName.clear();
    segsOfStreet.clear();
    intersectionsOfStreet.clear();
    lengthOfSegment.clear();
    travelTimeOfSegment.clear();
    segsOfIntersection.clear();
    adjacentSegIntsOfInt.clear();
    hlData.highlightedInts.clear(); 
    hlData.highlightedSegs.clear();
    indexesOfStreams.clear();
    areaOfFeatures.clear();
    indexesOfBuildingTypes.clear();
    segsOfStreetType.clear();
    segsOfWayOSMID.clear();
    nodeIndexOfOSMID.clear();
    wayIndexOfOSMID.clear();
    relationIndexOfOSMID.clear();
}

//==============================================================================
// Initializers
//==============================================================================

// Sizes street vectors to their appropriate size to avoid out of index access
void MapData::allocStreetVecs(const unsigned& numStreets) {
    segsOfStreet.resize(numStreets);
    intersectionsOfStreet.resize(numStreets);
    segsOfStreetType.resize(roadType::RT_TYPECOUNT);
    indexesOfBuildingTypes.resize(buildingType::BF_TYPECOUNT);
}

// Sizes segment vectors to their appropriate size to avoid out of index access
void MapData::allocSegmmentVecs(const unsigned& numSegments) {
    lengthOfSegment.resize(numSegments);
    travelTimeOfSegment.resize(numSegments);
}

// Sizes intersection vectors to their appropriate size to avoid out of index access
void MapData::allocIntersectionVecs(const unsigned& numIntersections) {
    segsOfIntersection.resize(numIntersections);
    adjacentSegIntsOfInt.resize(numIntersections);
}

// Sets map path
void MapData::setMapPath(std::string _mapPath) {
    mapPath = _mapPath;
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

// Calculates length and travel time of segment and adds to vectors indexed by the segID
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
            // Else from last curve point to last intersection
            else if (i == SSData.curvePointCount)
                dist += find_distance_between_two_points(std::make_pair
                        (getStreetSegmentCurvePoint(numCurves - 1, segID), toInt));
            // Else from one curve point to the next curve point
            else
                dist += find_distance_between_two_points(std::make_pair
                        (getStreetSegmentCurvePoint(i - 1, segID), 
                         getStreetSegmentCurvePoint(i, segID)));       
        }
    }
    // Stores distance of segment
    lengthOfSegment[segID] = dist;
    
    // Calculates and stores travel time
    travelTimeOfSegment[segID] = dist * (1 / SSData.speedLimit) * 3.6;
}

// Adds intersectionID to a unordered_set inside a vector indexed to its streetID
void MapData::addIntersectToStreet(const IntersectionIndex& intID, const StreetIndex& streetID) {
    intersectionsOfStreet[streetID].push_back(intID);
}

// Adds segment to vector containing all segments inside a vector indexed to its intersectionID
void MapData::addSegToIntersection(const StreetSegmentIndex& segID, const IntersectionIndex& intID) {
    segsOfIntersection[intID].push_back(segID);
}

// Adds adjacent intersection to set with all reachable intersections from main intersection
void MapData::addAdjacentPairSegIntIdToInt(const std::vector<pairSegIntID>& adjacentSegInts,
                                           const IntersectionIndex& mainIntID) {
    adjacentSegIntsOfInt[mainIntID] = adjacentSegInts;
}

//
void MapData::addHighlightedSegs(const std::vector<StreetSegmentIndex>& segs) {
    hlData.highlightedSegs = segs;
}

// Adds an intersection that is to be highlighted
void MapData::addHighlightedInt(const IntersectionIndex& intID) {
    hlData.highlightedInts.push_back(intID);
}

// Adds an intersection that is to be highlighted at the front of the vector
void MapData::addHighlightedIntAtFront(const IntersectionIndex& intID) {
    hlData.highlightedInts.insert(hlData.highlightedInts.begin(), intID);
}

// Removes the last highlighted intersection
void MapData::removeLastHighlightedInt(){
    if (!hlData.highlightedInts.empty()){
        hlData.highlightedInts.erase(hlData.highlightedInts.end() - 1); 
    }
}

// Removes the first highlighted intersection
void MapData::removeFirstHighlightedInt() {
    if (!hlData.highlightedInts.empty()) {
        hlData.highlightedInts.erase(hlData.highlightedInts.begin());
    }
}

// Sets whether or not a starting intersection is highlighted
void MapData::setStartHighlight(bool highlighted) {
    hlData.startHighlighted = highlighted;
}

// Sets whether or not a destination intersection is highlighted
void MapData::setDesintationHighlight(bool highlighted) {
    hlData.destinationHighlighted = highlighted;
}

// Stores the max and min lat/lon
void MapData::addCoordData(const double& _minLat, const double& _maxLat, 
                           const double& _minLon, const double& _maxLon) {
    coordData.minLat = _minLat;
    coordData.maxLat = _maxLat;
    coordData.minLon = _minLon;
    coordData.maxLon = _maxLon;
    coordData.avgLat = (_minLat + _maxLat) * 0.5;
    coordData.avgLon = (_minLon + _maxLon) * 0.5;
    coordData.latAspectRatio = cos(coordData.avgLat * DEGREE_TO_RADIAN);
}

// Adds index of stream to be stored
void MapData::addIndexOfStream(const FeatureIndex& streamID) {
    indexesOfStreams.push_back(streamID);
}

// Adds previously calculated area to be stored
void MapData::addAreaOfFeature(const double& area, const FeatureIndex& featureID) {
    areaOfFeatures.insert(std::make_pair(area, featureID));
}

// Adds indexes of buildings by their given building type
void MapData::addIndexOfBuildingType(const FeatureIndex& buildingID, const buildingType& type) {
    indexesOfBuildingTypes[type].push_back(buildingID);
}

// Adds segment index of a given street type
void MapData::addSegOfStreetType(const StreetSegmentIndex& segID, const unsigned& numLanes, 
                                 const roadType& type) {
    segsOfStreetType[type].push_back(std::make_pair(segID, numLanes));
}

// Adds a segments way OSMID to a map keyed to its segment ID
void MapData::addSegToWayOSMID(const StreetSegmentIndex& segID, const OSMID& wayID) {
    const auto itr = segsOfWayOSMID.find(wayID);
    if (itr == segsOfWayOSMID.end()) {
        std::vector<int> segs{ segID };
        segsOfWayOSMID.insert(std::make_pair(wayID, segs));
    }
    else 
        itr->second.push_back(segID);
}

// Adds node index (0 to numNodes) to map keyed to its OSMID
void MapData::addNodeIndexToOSMID(const unsigned& nodeIndex, const OSMID& nodeID) {
    nodeIndexOfOSMID.insert(std::make_pair(nodeID, nodeIndex));
}

// Adds way index (0 to numWays) to map keyed to its OSMID
void MapData::addWayIndexToOSMID(const unsigned& wayIndex, const OSMID& wayID) {
    wayIndexOfOSMID.insert(std::make_pair(wayID, wayIndex));
}

// Adds a relation index to a map keyed to its OSMID
void MapData::addRelationIndexToOSMID(const unsigned& relationIndex, const OSMID& relationID) {
    relationIndexOfOSMID.insert(std::make_pair(relationID, relationIndex));
}

// Sets pointer to current intersection info dialog box
void MapData::setIntersectionInfoBox(GtkWidget* dialog) {
    intersectionDialog = dialog;    
}

//==============================================================================
// Accessors
//==============================================================================

// Returns vector containing all streetIDs corresponding to the name given
// Works with partial names and ignores spaces (e.g. "dund a" for "Dundas st.")
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
const std::vector<int>& MapData::getSegsOfStreet(const StreetIndex& streetID) const {
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
    return intersectionsOfStreet[streetID];
}

// Returns vector containing IDs of all segments at a given intersection
const std::vector<int>& MapData::getSegsOfIntersection(const IntersectionIndex& intID) const {
    return segsOfIntersection[intID];
}

// Returns vector containing IDs of all intersections reachable from a given intersection
const std::vector<pairSegIntID> MapData::getAdjacentSegIntIDsOfInt(const IntersectionIndex& intID) const {
    return adjacentSegIntsOfInt[intID];
}

// Returns the cosine of the average latitude
double MapData::getLatAspectRatio() const {
    return coordData.latAspectRatio;
}

// Functions to return the min or max latitude
double MapData::getMinLat() const {
    return coordData.minLat;
}
double MapData::getMaxLat() const {
    return coordData.maxLat;
}

// Functions to return the min or max longitude
double MapData::getMinLon() const {
    return coordData.minLon;
}
double MapData::getMaxLon() const {
    return coordData.maxLon;
}

// Functions to return the average latitude or longitude
double MapData::getAvgLat() const {
    return coordData.avgLat;
}
double MapData::getAvgLon() const {
    return coordData.avgLon;
}

// Returns the data to be highlighted by the renderer
const HighlightedData& MapData::getHLData() const {
    return hlData;
}

// Returns whether or not a starting intersection is highlighted
bool MapData::isStartHighlighted() {
    return hlData.startHighlighted;
}
    
// Returns whether or not a destination intersection is highlighted
bool MapData::isDestinationHighlighted() {
    return hlData.destinationHighlighted;
}

// Returns the area of all features in a sorted multimap
const std::multimap<double, unsigned>& MapData::getAreaOfFeatures() const {
    return areaOfFeatures;
}

// Returns indexes of all streams on our map
const std::vector<unsigned>& MapData::getIndexesOfStreams() const {
    return indexesOfStreams;
}

// Returns indexes of all buildings of a given type
const std::vector<unsigned>& MapData::getIndexesOfBuildingType(const buildingType& type) const {
    return indexesOfBuildingTypes[type];
}

// Gets all indexes of segments of a street type
const std::vector<std::pair<int, unsigned> >& MapData::getSegsOfStreetType(const roadType& type) const {
    return segsOfStreetType[type];
}

// Gets all segments corresponding to a way OSMID
const std::vector<int> MapData::getSegsOfWayOSMID(const OSMID& wayID) const {
    const auto itr = segsOfWayOSMID.find(wayID);
    if (itr != segsOfWayOSMID.end())
        return itr->second;
    else {
        std::vector<int> emptyVec;
        return emptyVec;
    }
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
        std::cerr << "No way found with OSMID " << wayID << "\n";
        return 0;
    }
    // Found a valid OSMID. Return the way index of the OSMID
    else
        return mapItr->second;
}

// Returns all relations that correspond to a given OSMID
unsigned MapData::getRelationIndexOfOSMID(const OSMID& relationID) const {
    const auto mapItr = relationIndexOfOSMID.find(relationID);
    // If no OSMID match is found the itr will point to end
    if (mapItr == relationIndexOfOSMID.end()) {
        std::cerr << "No relation found with OSMID " << relationID << "\n";
        return 0;
    }
    // Found a valid OSMID. Return the relation index of the OSMID
    else
        return mapItr->second;
}


// Returns map path
const std::string MapData::getMapPath() const {
    return mapPath;
}

// Returns pointer to dialog box displaying currently clicked on intersection
GtkWidget* MapData::getIntersectionInfoBox() const {
    return intersectionDialog;
}
