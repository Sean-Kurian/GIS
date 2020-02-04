//==============================================================================
// File Description: This file implements the load_map and close_map functions
// along with some sub-functions that they used. These are used to load all the
// map data that we need into data structures in the global data object
// Close map clears these structures which allows us to open another map
//==============================================================================

#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "globalData.h"

#include <algorithm>
#include <regex>

MapData gData; // Global data object used to store data in load map

// Forward declarations of our load_map sub functions
void getStreetData(const unsigned& numStreets);
void getSegmentData(const unsigned& numStreetSegments);
void getIntersectionData(const unsigned& numIntersections);
void getLayer1Data(const unsigned& numNodes, const unsigned& numWays);

// Loads layer 1 and 2 data into gData's data structures
bool load_map(std::string mapPath) {
    bool loadSuccessful = loadStreetsDatabaseBIN(mapPath);
    
    if (loadSuccessful) {
        const unsigned numStreets = getNumStreets();
        const unsigned numSegments = getNumStreetSegments();
        const unsigned numIntersections = getNumIntersections();
        
        // Sizes vectors in gData to correct size to avoid indexing errors
        gData.allocStreetVecs(numStreets);
        gData.allocSegmmentVecs(numSegments);
        gData.allocIntersectionVecs(numIntersections);
        
        // Loops over all of the respective elements and stores data in gData
        getStreetData(numStreets);
        getSegmentData(numSegments);
        getIntersectionData(numIntersections);
    }
    // Changes "streets" to "osm" to load layer 1 data
    mapPath = std::regex_replace(mapPath, std::regex("streets"), "osm");
    loadSuccessful = loadOSMDatabaseBIN(mapPath);
    
    if (loadSuccessful) {
        const unsigned numNodes = getNumberOfNodes();
        const unsigned numWays = getNumberOfWays();
        // Loops over all nodes and ways to store OSMIDs keyed to their index
        getLayer1Data(numNodes, numWays);
    }
    return loadSuccessful;
}

// Clears all data to be able to open another map without restarting program
void close_map() {
    // Clears all data in gData structures but doesn't destroy object
    gData.clearMapData();
    closeOSMDatabase();
    closeStreetDatabase();
}

// Loads street data into gData by looping over all streets
void getStreetData(const unsigned& numStreets) {
    std::string streetName;
    for (unsigned streetIndex =  0; streetIndex < numStreets; ++streetIndex) {
        streetName = getStreetName(streetIndex);
        // In-place removal of spaces from street name
        streetName.erase(std::remove_if(streetName.begin(), streetName.end(), ::isspace), 
                                        streetName.end());
        // In-place transform to lower case characters
        std::transform(streetName.begin(), streetName.end(), streetName.begin(), 
                       [](unsigned char letter){ return std::tolower(letter); });
        // Adds street name to multimap which pairs it with its street index
        gData.addStreetIDtoName(streetIndex, streetName);
    }
}

// Loads segment data into gData by looping over all segments
void getSegmentData(const unsigned& numStreetSegments) {
    InfoStreetSegment SSData;
    for (unsigned segIndex = 0; segIndex < numStreetSegments; ++segIndex) {
        SSData = getInfoStreetSegment(segIndex);
        // Adds segment to vector containing all segments of a street
        gData.addSegToStreet(segIndex, SSData.streetID);
        // Calculates and stores length and travel time of segment
        gData.addLengthAndTravelTimeOfSeg(SSData, segIndex);
        // Adds intersection to set with all intersections of a street
        gData.addIntersectToStreet(SSData.from, SSData.streetID);
        gData.addIntersectToStreet(SSData.to, SSData.streetID);
    }
}

// Loads intersection data into gData by looping over all intersections
void getIntersectionData(const unsigned& numIntersections) {
    InfoStreetSegment SSData;
    for (unsigned intIndex = 0; intIndex < numIntersections; ++intIndex) {
        // Finds and loops over all segments connected to intersection
        int numSegs = getIntersectionStreetSegmentCount(intIndex);
        for (unsigned segNum = 0; segNum < numSegs; ++segNum) {
            StreetSegmentIndex segIndex = getIntersectionStreetSegment(intIndex, segNum);
            // Adds segment to vector containing all segments at an intersection
            gData.addSegToIntersection(segIndex, intIndex);
            
            SSData = getInfoStreetSegment(segIndex);
            // If this is the starting intersection the end one must be reachable
            if (intIndex == SSData.from)
                gData.addAdjacentIntToIntersection(SSData.to, intIndex);
            // If its a two way street then the other intersection must be reachable
            else if (!SSData.oneWay)
                gData.addAdjacentIntToIntersection(SSData.from, intIndex);
        }
    }
}

// Loads maps in gData which connect the node/way indexes with their OSMIDs
void getLayer1Data(const unsigned& numNodes, const unsigned& numWays) {
    const OSMNode* node;
    const OSMWay* way;
    // Loops over all nodes and stores their index with their OSMID
    for (unsigned nodeIndex = 0; nodeIndex < numNodes; ++nodeIndex) {
        node = getNodeByIndex(nodeIndex);
        gData.addNodeIndexToOSMID(nodeIndex, node->id());
    }
    // Loops over all nodes and stores their index with their OSMID
    for (unsigned wayIndex = 0; wayIndex < numWays; ++wayIndex) {
        way = getWayByIndex(wayIndex);
        gData.addWayIndexToOSMID(wayIndex, way->id());
    }
}