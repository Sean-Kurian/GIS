#ifndef MAPDATA_H
#define MAPDATA_H

//==============================================================================
// File Description:
//
//==============================================================================

#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>

class MapData {
private:
    // Multimap which stores street names keyed to their street values
    // Using multimap for O(logn) searching and O(logn) insertion
    std::multimap<std::string, unsigned> IDsOfStreetName;
    
    // Vector which stores all street segments of each street
    // Using vector & set for O(1) access. Using set to guarantee no duplicates
    std::vector<std::vector<int> > segsOfStreet;
    
    // Vector which stores set of all unique intersections along any street
    // Using vector & set for O(1) access. Using set to guarantee no duplicates
    std::vector<std::unordered_set<int> > intersectionsOfStreet;
    
    // Vector which stores the length of every segment indexed to its segmentID
    std::vector<double> lengthOfSegment;
    
    //Vector which stores the travel time of every segment indexed to its segmentID
    std::vector<double> travelTimeOfSegment;
    
    // Vector which stores all segments of any intersection in a vector
    std::vector<std::vector<int> > segsOfIntersection;
    
    //
    std::vector<std::vector<int> > adjacentIntsOfIntersection; 
    
    // Unordered map which stores the node indexes of OSMIDs in key, value pairs
    // Unordered map has O(1) insertion/access. Not using vector because can't index an OSMID
    std::unordered_map<OSMID, unsigned> nodeIndexOfOSMID;
    
    // Unordered map which stores the way indexes of OSMIDs in key, value pairs
    // Unordered map has O(1) insertion/access. Not using vector because can't index an OSMID   
    std::unordered_map<OSMID, unsigned> wayIndexOfOSMID;
    
public:
//==============================================================================
// Constructors / Destructors
//==============================================================================
    MapData();
    ~MapData();
    
    // Clears all structures used to store data. Used to load map without needing destructor
    void clearMapData();
//==============================================================================
// Initializers
//==============================================================================
    // Sizes vectors to their appropriate size to avoid out of index access
    void allocStreetVecs(const unsigned& numStreets);
    void allocSegmmentVecs(const unsigned& numSegments);
    void allocIntersectionVecs(const unsigned& numIntersections);
//==============================================================================
// Mutators
//==============================================================================
    // Adds streetID to multimap which is keyed to its street name
    void addStreetIDtoName(const StreetIndex& streetID, const std::string& streetName);
    
    // Adds segment to an unordered_set inside a vector indexed to its streetID
    void addSegToStreet(const StreetSegmentIndex& segID, const StreetIndex& streetID);
    
    // Calculates length and travel time of seg and adds to vectors indexed by the segID
    void addLengthAndTravelTimeOfSeg(const InfoStreetSegment& SSData, const unsigned& segID);
    
    // Adds intersectionID to a unordered_set inside a vector indexed to its streetID 
    void addIntersectToStreet(const IntersectionIndex& intID, const StreetIndex& streetID);
    
    // Adds segment to vector containing all segments inside a vector indexed to its intersectionID
    void addSegToIntersection(const StreetSegmentIndex& segID, const IntersectionIndex& intID);
    
    //
    void addAdjacentIntToIntersection(const IntersectionIndex& adjacentIntID, 
                                      const IntersectionIndex& mainIntID);
    
    // Adds node index (0 to numNodes) to map keyed to its OSMID
    void addNodeIndexToOSMID(const unsigned& nodeIndex, const OSMID& nodeID);
    
    // Adds way index (0 to numWays) to map keyed to its OSMID
    void addWayIndexToOSMID(const unsigned& wayIndex, const OSMID& wayID);
//==============================================================================
// Accessors
//==============================================================================
    // Returns vector containing all streetIDs corresponding to the name given
    // Works with partial names and ignores spaces (e.g. "dund a" for "Dundas st.")
    const std::vector<int> getStreetIDsFromStreetName(std::string name) const;
    
    // Returns a vector containing IDs of all segments along a street
    const std::vector<int> getSegsOfStreet(const StreetIndex& streetID) const;
    
    // Returns length of a given segment
    double getLengthOfSegment(const StreetSegmentIndex& segID) const;
    
    // Returns time needed to travel at speed limit down a given segment
    double getTravelTimeOfSegment(const StreetSegmentIndex& segID) const;
    
    // Returns vector containing IDs of all intersections along a street
    const std::vector<int> getIntersectionsOfStreet(const StreetIndex& streetID) const;
    
    // Returns vector containing IDs of all segments at a given intersection
    const std::vector<int> getSegsOfIntersection(const IntersectionIndex& intID) const;
    
    //
    const std::vector<int> getAdjacentIntsOfIntersection(const IntersectionIndex& intID) const;
    
    // Returns node index (0 to numNodes) of the OSMID. Outputs error if none found
    unsigned getNodeIndexOfOSMID(const OSMID& nodeID) const;
    
    // Returns way index (0 to numWays) of the OSMID. Outputs error if none found
    unsigned getWayIndexOfOSMID(const OSMID& wayID) const;
};

#endif /* MAPDATA_H */