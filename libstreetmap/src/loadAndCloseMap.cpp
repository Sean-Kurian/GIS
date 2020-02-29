//==============================================================================
// File Description: This file implements the load_map and close_map functions
// along with some sub-functions that they use. These are used to load all the
// map data that we need into data structures in the global data object
// Close map clears these structures which allows us to open another map
//==============================================================================

#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "globalData.h"
#include "roadTypes.h"
#include "naturalFeatures.h"

#include <algorithm>
#include <regex>

MapData gData; // Global data object used to store data in load map

// Forward declarations of our load_map sub functions
void getStreetData(const unsigned& numStreets);
void getSegmentData(const unsigned& numStreetSegments);
void getIntersectionData(const unsigned& numIntersections);
void getFeatureData(const unsigned& numFeatures);
void getLayer1Data(const unsigned& numNodes, const unsigned& numWays, 
                   const unsigned& numRelations);

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
        
        //Stores the map path
        gData.setMapPath(mapPath);
    }
    // Changes "streets" to "osm" to load layer 1 data
    mapPath = std::regex_replace(mapPath, std::regex("streets"), "osm");
    loadSuccessful = loadOSMDatabaseBIN(mapPath);
    
    if (loadSuccessful) {
        const unsigned numNodes = getNumberOfNodes();
        const unsigned numWays = getNumberOfWays();
        const unsigned numRelations = getNumberOfRelations();
        const unsigned numFeatures = getNumFeatures();
        // Loops over all nodes and ways to store OSMIDs keyed to their index
        getLayer1Data(numNodes, numWays, numRelations);
        //
        getFeatureData(numFeatures);
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
        //
        gData.addSegToWayOSMID(segIndex, SSData.wayOSMID);
    }
}

// Loads intersection data into gData by looping over all intersections
void getIntersectionData(const unsigned& numIntersections) {
    InfoStreetSegment SSData;
    for (unsigned intIndex = 0; intIndex < numIntersections; ++intIndex) {
        std::vector<int> streetIDs; 
        // Finds and loops over all segments connected to intersection
        int numSegs = getIntersectionStreetSegmentCount(intIndex);
        for (unsigned segNum = 0; segNum < numSegs; ++segNum) {
            bool added = false;
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
            
            // Check if this streetID has already been added to intersectsOfStreet
            for (const int& ID : streetIDs) {
                if (ID == SSData.streetID)
                    added = true;
            }
            streetIDs.push_back(SSData.streetID);
            if (!added)
                // Adds intersection to vector with all intersections of a street
                gData.addIntersectToStreet(intIndex, SSData.streetID);
        }
    }
}

//
void getFeatureData(const unsigned& numFeatures) {
    LatLon location = getFeaturePoint(0, 0);
    double minLat = location.lat();
    double minLon = location.lon();
    double maxLat = minLat;
    double maxLon = minLon;
    
    for (unsigned featureIndex = 0; featureIndex < numFeatures; ++featureIndex) {
        unsigned numPoints = getFeaturePointCount(featureIndex);
        for (unsigned pointIndex = 0; pointIndex < numPoints; ++pointIndex) {
            location = getFeaturePoint(pointIndex, featureIndex);
            minLat = std::min(minLat, location.lat());
            minLon = std::min(minLon, location.lon());
            maxLat = std::max(maxLat, location.lat());
            maxLon = std::max(maxLon, location.lon());
        }
        naturalFeature type = determineNaturalFeature(featureIndex);
        if (getFeatureName(featureIndex) == "The Lake") {
            std::cout << "The Lake: " << featureIndex << " " << numPoints << "\n";
            type = naturalFeature::minorWater;
        }
        if (type != naturalFeature::NF_TYPECOUNT)
            gData.addIndexOfNaturalFeature(featureIndex, type);
        else
            gData.addIndexOfBuilding(featureIndex);
    }
    gData.addCoordData(minLat, maxLat, minLon, maxLon);
}

// Loads maps in gData which connect the node/way indexes with their OSMIDs
void getLayer1Data(const unsigned& numNodes, const unsigned& numWays, const unsigned& numRelations) {
    const OSMNode* node;
    const OSMWay* way;
    const OSMRelation* relation;
    // Loops over all nodes and stores their index with their OSMID
    for (unsigned nodeIndex = 0; nodeIndex < numNodes; ++nodeIndex) {
        node = getNodeByIndex(nodeIndex);
        gData.addNodeIndexToOSMID(nodeIndex, node->id());
    }
    // Loops over all ways 
    for (unsigned wayIndex = 0; wayIndex < numWays; ++wayIndex) {
        way = getWayByIndex(wayIndex);
        // Stores way's index with its OSMID
        gData.addWayIndexToOSMID(wayIndex, way->id());
        std::vector<int> segs = gData.getSegsOfWayOSMID(way->id());
        unsigned numLanes = 2;
        roadType type;
        
        for (unsigned tagNum = 0; tagNum < getTagCount(way); ++tagNum) {
            std::string key, val;
            std::tie(key, val) = getTagPair(way, tagNum);
            if (key == "lanes") {
                try {
                    numLanes = std::stoi(val);
                }
                catch (const std::exception& e) {
                    std::cerr << "Exception thrown when determining number of lanes\n"
                              << "Value: " << val << "\n"
                              << "Exception: " << e.what() << "\n";
                }
            }
            else if (key == "highway") 
                type = determineRoadType(val);
        }
        for (const int segIndex : segs)
            gData.addSegToStreetType(segIndex, numLanes, type);
    }
    //
    for (unsigned relationIndex = 0; relationIndex < numRelations; ++relationIndex) {
        relation = getRelationByIndex(relationIndex);
        gData.addRelationIndexToOSMID(relationIndex, relation->id());
    }
}

roadType determineRoadType(const std::string& val) {
    if (val == "residential" || val == "unclassified" || val == "living_street"
            || val == "service" || val == "road")
        return roadType::minorRoad;
    
    else if (val == "secondary" || val == "tertiary" || val == "secondary_link"  
            || val == "tertiary_link")
        return roadType::majorRoad;
    
    else if (val == "motorway" || val == "trunk" || val == "primary" || val == "motorway_link" 
            || val == "trunk_link" || val == "primary_link")
        return roadType::highway;
    
    else if (val == "track" || val == "path")
        return roadType::trail;
    
    else if (val == "pedestrian" || val == "footway" || val == "steps")
        return roadType::path;
    
    return roadType::minorRoad;
}

naturalFeature determineNaturalFeature(const unsigned& featureIndex) {
    FeatureType type = getFeatureType(featureIndex);
        
    if (type == FeatureType::Building)
        return naturalFeature::NF_TYPECOUNT;
    
    else if (type == FeatureType::Greenspace)
        return naturalFeature::forest;
    
    else if (type == FeatureType::Park || type == FeatureType::Golfcourse)
        return naturalFeature::park;
    
    else if (type == FeatureType::Island)
        return naturalFeature::island;
    
    else if (type == FeatureType::Beach)
        return naturalFeature::beach;
    
    else if (type == FeatureType::Lake) {
        return naturalFeature::lake;
    }
        
    else if (type == FeatureType::River)
        return naturalFeature::river;
    
    else if (type == FeatureType::Stream)
        return naturalFeature::minorWater;
    
    return naturalFeature::forest;
}


//    else if (type == FeatureType::Lake) {
//        const OSMEntity* feature = NULL;
//        TypedOSMID featureOSM = getFeatureOSMID(featureIndex);
//        if (featureOSM.type() == TypedOSMID::EntityType::Node)
//            feature = getNodeByIndex(gData.getNodeIndexOfOSMID(featureOSM));
//        else if (featureOSM.type() == TypedOSMID::EntityType::Way)
//            feature = getWayByIndex(gData.getWayIndexOfOSMID(featureOSM));
//        else if (featureOSM.type() == TypedOSMID::EntityType::Relation)
//            feature = getRelationByIndex(gData.getRelationIndexOfOSMID(featureOSM));
//        
//        if (feature != NULL) {
//            for (unsigned tagNum = 0; tagNum < getTagCount(feature); ++tagNum) {
//                std::string key, val;
//                std::tie(key, val) = getTagPair(feature, tagNum);
//
//                if (key == "water") {
//                    if (val == "pond" || val == "reservoir" || val == "lake;pond")
//                        return naturalFeature::minorWater;
//                    else 
//                        return naturalFeature::lake;
//                }
//            }
//        }
//        else
//            return naturalFeature::lake;
//    }