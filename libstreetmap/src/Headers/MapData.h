#ifndef MAPDATA_H
#define MAPDATA_H

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
    std::multimap<std::string, unsigned> IDsOfStreetNames;
    
    // Vector which stores set of all unique intersections along any street
    // Using vector & set for O(1) access. Using set to guarantee no duplicates
    std::vector<std::unordered_set<int> > intersectionsOfStreet;
    
    // Vector which stores all segments of any intersection in a vector
    std::vector<std::vector<int> > segsOfIntersection;

    // Vector which stores all street segments of each street
    // Using vector & set for O(1) access. Using set to guarantee no duplicates
    std::vector<std::unordered_set<int> > segsOfStreet;
    
    // 
    std::unordered_map<OSMID, unsigned> nodeIndexOfOSMID;
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
    void allocSegOfStreetVecs(const unsigned& numStreets);
//==============================================================================
// Mutators
//==============================================================================
    // Adds streetID to multimap which is keyed to its street name
    void addStreetIDtoName(const StreetIndex& streetID, const std::string& streetName);
    
    // Adds intersectionID to a unordered_set inside a vector indexed to its streetID 
    void addIntersectToStreet(const IntersectionIndex& intID, const StreetIndex& streetID);
    
    // Adds segment to vector containing all segments inside a vector indexed to its intersectionID
    void addSegToIntersection(const StreetSegmentIndex& segID, const IntersectionIndex& intID);

    // Adds segment to an unordered_set inside a vector indexed to its streetID
    void addSegToStreet(const StreetSegmentIndex& segID, const StreetIndex& streetID);

    
    //
    void addNodeIndexToOSMID(const unsigned& nodeIndex, const OSMID& nodeID);
//==============================================================================
// Accessors
//==============================================================================
    // Returns vector containing all streetIDs corresponding to the name given
    // Works with partial names and ignores spaces (e.g. dund a for Dundas st.)
    const std::vector<int> getStreetIDsFromStreetName(std::string name) const;
    
    // Returns vector containing IDs of all intersections along a street
    const std::vector<int> getIntersectionsOfStreet(const StreetIndex& streetID) const;
    
    // Returns vector containing IDs of all segments at a given intersection
    const std::vector<int> getSegsOfIntersection(const IntersectionIndex& intID) const;

    // Returns a vector containing IDs of all segments along a street
    const std::vector<int> getSegmentsOfStreet(const StreetIndex& streetID) const;

    
    //
    const unsigned getNodeIndexOfOSMID(const OSMID& nodeID) const;
};

#endif /* MAPDATA_H */