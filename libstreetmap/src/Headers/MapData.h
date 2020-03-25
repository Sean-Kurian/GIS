#ifndef MAPDATA_H
#define MAPDATA_H

//==============================================================================
// File Description: Class header of the global object used to store all data
// needed for the map during load_map. Mutators should only be used in load_map
// Accessors are used to get data from class outside of load_map
//==============================================================================

#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "CoordData.h"
#include "HighlightedData.h"
#include "roadTypes.h"
#include "buildingTypes.h"

#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

#include "ezgl/graphics.hpp"
#include "ezgl/application.hpp"

class MapData {
private:
    // Multimap which stores street names keyed to their street values
    // Using multimap for O(logn) searching and O(logn) insertion
    std::multimap<std::string, unsigned> IDsOfStreetName;
    
    // Vector which stores all street segments of each street
    std::vector<std::vector<int> > segsOfStreet;
    
    // Vector which stores set of all unique intersections along any street
    // Using vector for O(1) access. Using set to guarantee no duplicates
    std::vector<std::vector<int> > intersectionsOfStreet;
    
    // Vector which stores the length of every segment indexed to its segmentID
    std::vector<double> lengthOfSegment;
    
    //Vector which stores the travel time of every segment indexed to its segmentID
    std::vector<double> travelTimeOfSegment;
    
    // Vector which stores all segments of any intersection in a vector
    std::vector<std::vector<int> > segsOfIntersection;
    
    // Vector which stores all reachable intersections from an intersection in a set
    // Using vector & unord set for O(1) access. Using set guarantees no duplicates
    std::vector<std::unordered_set<int> > adjacentIntsOfIntersection; 
    
    //
    HighlightedData hlData;
    
    //
    CoordData coordData;
    
    //
    std::multimap<double, unsigned> areaOfFeatures;
    
    //
    std::vector<unsigned> indexesOfStreams;
    
    //
    std::vector<std::vector<unsigned> > indexesOfBuildingTypes;
    
    //
    std::vector<std::vector<std::pair<int, unsigned> > > segsOfStreetType;
    
    //
    std::unordered_map<OSMID, std::vector<int> > segsOfWayOSMID;
    
    // Unordered map which stores the node indexes of OSMIDs in key, value pairs
    std::unordered_map<OSMID, unsigned> nodeIndexOfOSMID;
    
    // Unordered map which stores the way indexes of OSMIDs in key, value pairs
    std::unordered_map<OSMID, unsigned> wayIndexOfOSMID;
    
    // Unordered map which stores the relation indexes of OSMIDs in key, value pairs
    std::unordered_map<OSMID, unsigned> relationIndexOfOSMID;
    
    //String to store map_path
    std::string mapPath;
    
    //Pointer to current dialog box displaying intersection info
    GtkWidget* intersectionDialog;
    
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
    
    // Sets map path
    void setMapPath(std::string _mapPath);
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
    
    // Adds adjacent intersection to set with all reachable intersections from main intersection
    void addAdjacentIntToIntersection(const IntersectionIndex& adjacentIntID, 
                                      const IntersectionIndex& mainIntID);
    
    // Adds an intersection that is to be highlighted
    void addHighlightedInt(const IntersectionIndex& intID);
    
    // Adds an intersection that is to be highlighted at the front of the vector
    void addHighlightedIntAtFront(const IntersectionIndex& intID);
    
    // Removes the last highlighted intersection
    void removeLastHighlightedInt(); 
    
    // Removes the first highlighted intersection
    void removeFirstHighlightedInt();
    
    // Sets whether or not a starting intersection is highlighted
    void setStartHighlight(bool highlighted);
    
    // Sets whether or not a destination intersection is highlighted
    void setDesintationHighlight(bool highlighted);
    
    // Stores the max and min lat/lon
    void addCoordData(const double& _minLat, const double& _maxLat, 
                      const double& _minLon, const double& _maxLon);
    
    // Adds index of stream to be stored
    void addIndexOfStream(const FeatureIndex& streamID);
    
    // Adds previously calculated area to be stored
    void addAreaOfFeature(const double& area, const FeatureIndex& featureID);
    
    // Adds indexes of buildings by their given building type
    void addIndexOfBuildingType(const FeatureIndex& buildingID, const buildingType& type);
    
    // Adds segment index of a given street type
    void addSegOfStreetType(const StreetSegmentIndex& segID, const unsigned& numLanes, 
                            const roadType& type);
    
    // Adds a segments way OSMID to a map keyed to its segment ID
    void addSegToWayOSMID(const StreetSegmentIndex& segID, const OSMID& wayID);
    
    // Adds node index (0 to numNodes) to map keyed to its OSMID
    void addNodeIndexToOSMID(const unsigned& nodeIndex, const OSMID& nodeID);
    
    // Adds way index (0 to numWays) to map keyed to its OSMID
    void addWayIndexToOSMID(const unsigned& wayIndex, const OSMID& wayID);
    
    // Adds a relation index to a map keyed to its OSMID
    void addRelationIndexToOSMID(const unsigned& relationIndex, const OSMID& relationID);
    
    // Sets pointer to current intersection info dialog box
    void setIntersectionInfoBox(GtkWidget* dialog);
//==============================================================================
// Accessors
//==============================================================================
    // Returns vector containing all streetIDs corresponding to the name given
    // Works with partial names and ignores spaces (e.g. "dund a" for "Dundas st.")
    const std::vector<int> getStreetIDsFromStreetName(std::string name) const;
    
    // Returns a vector containing IDs of all segments along a street
    const std::vector<int>& getSegsOfStreet(const StreetIndex& streetID) const;
    
    // Returns length of a given segment
    double getLengthOfSegment(const StreetSegmentIndex& segID) const;
    
    // Returns time needed to travel at speed limit down a given segment
    double getTravelTimeOfSegment(const StreetSegmentIndex& segID) const;
    
    // Returns vector containing IDs of all intersections along a street
    const std::vector<int> getIntersectionsOfStreet(const StreetIndex& streetID) const;
    
    // Returns vector containing IDs of all segments at a given intersection
    const std::vector<int>& getSegsOfIntersection(const IntersectionIndex& intID) const;
    
    // Returns vector containing IDs of all intersections reachable from a given intersection
    const std::vector<int> getAdjacentIntsOfIntersection(const IntersectionIndex& intID) const;
    
    // Returns the cosine of the average latitude
    double getLatAspectRatio() const;
    
    // Return the min or max latitude
    double getMinLat() const;
    double getMaxLat() const;
    
    // Return the min or max longitude
    double getMinLon() const;
    double getMaxLon() const;
    
    // Return the average latitude or longitude
    double getAvgLat() const;
    double getAvgLon() const;
    
    // Returns the data to be highlighted by the renderer
    const HighlightedData& getHLData() const;
    
    // Returns whether or not a starting intersection is highlighted
    bool isStartHighlighted();
    
    // Returns whether or not a destination intersection is highlighted
    bool isDestinationHighlighted();
    
    // Returns the area of all features in a sorted multimap
    const std::multimap<double, unsigned>& getAreaOfFeatures() const;
    
    // Returns indexes of all streams on our map
    const std::vector<unsigned>& getIndexesOfStreams() const;
    
    // Returns indexes of all buildings of a given type
    const std::vector<unsigned>& getIndexesOfBuildingType(const buildingType& type) const;
    
    // Gets all indexes of segments of a street type
    const std::vector<std::pair<int, unsigned> >& getSegsOfStreetType(const roadType& type) const;
    
    // Gets all segments corresponding to a way OSMID
    const std::vector<int> getSegsOfWayOSMID(const OSMID& wayID) const;
    
    // Returns node index (0 to numNodes) of the OSMID. Outputs error if none found
    unsigned getNodeIndexOfOSMID(const OSMID& nodeID) const;
    
    // Returns way index (0 to numWays) of the OSMID. Outputs error if none found
    unsigned getWayIndexOfOSMID(const OSMID& wayID) const;
    
    // Returns all relations that correspond to a given OSMID
    unsigned getRelationIndexOfOSMID(const OSMID& relationID) const;
    
    // Returns map path
    const std::string getMapPath() const;
    
    // Returns pointer to dialog box displaying currently clicked on intersection
    GtkWidget* getIntersectionInfoBox() const;
};

#endif /* MAPDATA_H */