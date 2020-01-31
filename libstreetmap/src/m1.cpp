/* 
 * Copyright 2020 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

//==============================================================================
// File Description:
//
//==============================================================================

#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "globalData.h"
#include "math.h"

#include <set>
#include <algorithm>
#include <regex>

MapData gData;

void getSegmentData(const unsigned& numStreetSegments);
void getIntersectionData(const unsigned& numIntersections);
void getStreetData(const unsigned& numStreets);
void getSegmentOfStreetData(const unsigned& numStreets);
void getLayer1Data(const unsigned& numNodes, const unsigned& numWays);

bool load_map(std::string mapPath) {
    bool loadSuccessful = loadStreetsDatabaseBIN(mapPath);
    
    if (loadSuccessful) {
        const unsigned numStreets = getNumStreets();
        const unsigned numSegments = getNumStreetSegments();
        const unsigned numIntersections = getNumIntersections();
        
        gData.allocStreetVecs(numStreets);
        gData.allocSegmmentVecs(numSegments);
        gData.allocIntersectionVecs(numIntersections);
        gData.allocSegOfStreetVecs(numStreets);

        getStreetData(numStreets);
        getSegmentData(numSegments);
        getIntersectionData(numIntersections);
        getSegmentOfStreetData(numStreets);
    }
    // Changes "streets" to "osm" to load layer 1 data
    mapPath = std::regex_replace(mapPath, std::regex("streets"), "osm");
    loadSuccessful = loadOSMDatabaseBIN(mapPath);
    if (loadSuccessful) {
        const unsigned numNodes = getNumberOfNodes();
        const unsigned numWays = getNumberOfWays();
        getLayer1Data(numNodes, numWays);
    }
    return loadSuccessful;
}

void close_map() {
    //Clean-up your map related data structures here
    gData.clearMapData();
    closeOSMDatabase();
    closeStreetDatabase();
}

void getStreetData(const unsigned& numStreets) {
    std::string streetName;
    for (unsigned streetInd = 0; streetInd < numStreets; ++streetInd) {
        streetName = getStreetName(streetInd);
        // In-place removal of spaces from street name
        streetName.erase(std::remove_if(streetName.begin(), streetName.end(), ::isspace), 
                                        streetName.end());
        // In-place transform to lower case characters
        std::transform(streetName.begin(), streetName.end(), streetName.begin(), 
                       [](unsigned char letter){ return std::tolower(letter); });
        // Adds street name to multimap which pairs it with its street index
        gData.addStreetIDtoName(streetInd, streetName);
    }
}

void getSegmentData(const unsigned& numStreetSegments) {
    InfoStreetSegment SSData;
    
    for (unsigned segInd = 0; segInd < numStreetSegments; ++segInd) {
        SSData = getInfoStreetSegment(segInd);
        
        gData.addIntersectToStreet(SSData.from, SSData.streetID);
        gData.addIntersectToStreet(SSData.to, SSData.streetID);
    }
}

void getIntersectionData(const unsigned& numIntersections) {
    for (unsigned intInd = 0; intInd < numIntersections; ++intInd) {
        int numSegs = getIntersectionStreetSegmentCount(intInd);
        for (unsigned segNum = 0; segNum < numSegs; ++segNum) {
            StreetSegmentIndex segInd = getIntersectionStreetSegment(intInd, segNum);
            gData.addSegToIntersection(segInd, intInd);
        }
    }
}



//Function to store all street segments of every street (used for find_street_segments_of_street)
void getSegmentOfStreetData(const unsigned& numStreets) {
    //Perform the following to get all the street segments for each street
    for (unsigned street_id = 0; street_id < numStreets; ++street_id) {
        
        //Get all the intersections on the given street
        std::vector<int> intersectionsOfStreet = gData.getIntersectionsOfStreet(street_id);

        //Iterate over each intersection on the street
        for (int intersectionCount = 0; intersectionCount < intersectionsOfStreet.size(); ++intersectionCount) {
            //Get all the segments on the intersection
            IntersectionIndex int_id = intersectionsOfStreet[intersectionCount];
            std::vector<int> segmentsOfIntersection = gData.getSegsOfIntersection(int_id);

            //Check each segment of the intersection to ensure it belongs to given street
            for (int segmentCount = 0; segmentCount < getIntersectionStreetSegmentCount(int_id); ++segmentCount) {
                StreetSegmentIndex seg_id = segmentsOfIntersection[segmentCount];
                InfoStreetSegment segmentInfo = getInfoStreetSegment(seg_id);
                if (segmentInfo.streetID == street_id) {
                    gData.addSegToStreet(seg_id, street_id);
                }
            }
        }
    }
}

void getLayer1Data(const unsigned& numNodes, const unsigned& numWays) {
    const OSMNode* node;
    const OSMWay* way;
    for (unsigned nodeInd = 0; nodeInd < numNodes; ++nodeInd) {
        node = getNodeByIndex(nodeInd);
        gData.addNodeIndexToOSMID(nodeInd, node->id());
    }
    for (unsigned wayInd = 0; wayInd < numWays; ++wayInd) {
        way =  getWayByIndex(wayInd);
        gData.addWayIndexToOSMID(wayInd, way->id());
    }
}

//Returns the distance between two coordinates in meters
double find_distance_between_two_points(std::pair<LatLon, LatLon> points) {
    
    double latAvg = DEGREE_TO_RADIAN * ((points.first.lat() + points.second.lat()) * 0.5);   
    double xDiff = (DEGREE_TO_RADIAN * points.second.lon() * cos(latAvg)) - DEGREE_TO_RADIAN * (points.first.lon() * cos(latAvg)); 
    double yDiff = DEGREE_TO_RADIAN * (points.second.lat() - points.first.lat()); 
    
    return (EARTH_RADIUS_METERS * sqrt(xDiff*xDiff + yDiff*yDiff)); 
}

//Returns the length of the given street segment in meters
double find_street_segment_length(int street_segment_id) {
    InfoStreetSegment seg = getInfoStreetSegment(street_segment_id); 
    double dist = 0; 
   
    if (seg.curvePointCount == 0) 
        return find_distance_between_two_points(std::make_pair(getIntersectionPosition(seg.from), getIntersectionPosition(seg.to))); 
    
    for (unsigned i = 0; i <= seg.curvePointCount; i++) {       
        if (i == 0) 
            dist = dist + find_distance_between_two_points(std::make_pair
                (getIntersectionPosition(seg.from), getStreetSegmentCurvePoint(i, street_segment_id)));               
        else if (i == seg.curvePointCount) 
            dist = dist + find_distance_between_two_points(std::make_pair
                (getStreetSegmentCurvePoint(seg.curvePointCount-1, street_segment_id), getIntersectionPosition(seg.to)));
        else 
            dist = dist + find_distance_between_two_points(std::make_pair
                (getStreetSegmentCurvePoint(i-1, street_segment_id), getStreetSegmentCurvePoint(i, street_segment_id)));       
    }
    return dist; 
}

//Returns the travel time to drive a street segment in seconds 
//(time = distance/speed_limit)
double find_street_segment_travel_time(int street_segment_id) {
    return (find_street_segment_length(street_segment_id) / 
           (getInfoStreetSegment(street_segment_id).speedLimit / 3.6)); 
}

//Returns the nearest intersection to the given position
int find_closest_intersection(LatLon my_position) {
    //Start by assuming that the 0th intersection is the closest
    int closestIntersection = 0;
    double closestDistance = find_distance_between_two_points(std::make_pair(my_position, 
                                                              getIntersectionPosition(0)));
    //Check each intersection against the 0th intersection to see if it's closer
    for (int intersectionID = 1; intersectionID < getNumIntersections(); ++intersectionID) {
        double distance = find_distance_between_two_points(std::make_pair(my_position, getIntersectionPosition(intersectionID)));
        if (distance < closestDistance) {
            closestDistance = distance;
            closestIntersection = intersectionID;
        }
    }
    return closestIntersection;
}

//Returns the street segments for the given intersection 
std::vector<int> find_street_segments_of_intersection(int intersection_id) {
    return gData.getSegsOfIntersection(intersection_id);
}

//Returns the street names at the given intersection (includes duplicate street 
//names in returned vector)
std::vector<std::string> find_street_names_of_intersection(int intersection_id) {
    std::vector<std::string> streetNames;
    InfoStreetSegment SSData;
    // Gets all segments of the intersection
    std::vector<int> segsOfInt = gData.getSegsOfIntersection(intersection_id);
    
    // Loops through the segments and adds their names to return vector
    for (const auto& segment : segsOfInt) {
        SSData = getInfoStreetSegment(segment);
        streetNames.push_back(getStreetName(SSData.streetID));
    }
    return streetNames;
}

//Returns true if you can get from intersection_ids.first to intersection_ids.second using a single 
//street segment (hint: check for 1-way streets too)
//corner case: an intersection is considered to be connected to itself
bool are_directly_connected(std::pair<int, int> intersection_ids) {
    
    if (intersection_ids.first == intersection_ids.second) 
        return true; 
    
    std::vector<int> adjacentIntersections = find_adjacent_intersections(intersection_ids.first); 
    for (const auto& intersection : adjacentIntersections){
        if (intersection == intersection_ids.second) 
            return true; 
    }
    return false; 
}

//Returns all intersections reachable by traveling down one street segment 
//from given intersection (hint: you can't travel the wrong way on a 1-way street)
//the returned vector should NOT contain duplicate intersections
std::vector<int> find_adjacent_intersections(int intersection_id) {
    std::set<int> adjacentIntersectionsSet;
    //Find all the segments attached to the intersection
    std::vector<int> segmentsOfIntersection = gData.getSegsOfIntersection(intersection_id);
    
    //Iterate through each segment
    for (auto it = segmentsOfIntersection.begin(); it != segmentsOfIntersection.end(); ++it) {
        //Get the info for the street segment
        InfoStreetSegment segmentInfo = getInfoStreetSegment(*it);
        //Avoid adding the given intersection
        if (segmentInfo.to != intersection_id) {
            adjacentIntersectionsSet.insert(segmentInfo.to);
        }
        //Ensure any from intersections added are not one-way segments as well
        if (segmentInfo.from != intersection_id && !segmentInfo.oneWay) {
            adjacentIntersectionsSet.insert(segmentInfo.from);
        }
    }
    //Create a vector to return
    std::vector<int> adjacentIntersections(adjacentIntersectionsSet.begin(), adjacentIntersectionsSet.end());
    return adjacentIntersections;
}

//Returns all street segments for the given street
std::vector<int> find_street_segments_of_street(int street_id) {
    return gData.getSegmentsOfStreet(street_id);
}

//Returns all intersections along the a given street
std::vector<int> find_intersections_of_street(int street_id) {
    return gData.getIntersectionsOfStreet(street_id);
}

//Return all intersection ids for two intersecting streets
//This function will typically return one intersection id.
//Made the choice not to use set_intersection as requires sorted arrays
std::vector<int> find_intersections_of_two_streets(std::pair<int, int> street_ids) {
    std::vector<int> streetOne = find_intersections_of_street(street_ids.first); 
    std::vector<int> streetTwo = find_intersections_of_street(street_ids.second); 
    
    std::vector<int> intersections(streetOne.size() + streetTwo.size()); 
    std::vector<int>::iterator it;
    
    std::sort(streetOne.begin(), streetOne.end()); 
    std::sort(streetTwo.begin(), streetTwo.end()); 
   
    it=std::set_intersection(streetOne.begin(), streetOne.end(), 
                          streetTwo.begin(), streetTwo.end(), intersections.begin()); 
    intersections.resize(it-intersections.begin());  
    return intersections;
}

//Returns all street ids corresponding to street names that start with the given prefix
//The function should be case-insensitive to the street prefix. You should ignore spaces.
//For example, both "bloor " and "BloOrst" are prefixes to "Bloor Street East".
//If no street names match the given prefix, this routine returns an empty (length 0) 
//vector.
//You can choose what to return if the street prefix passed in is an empty (length 0) 
//string, but your program must not crash if street_prefix is a length 0 string.
std::vector<int> find_street_ids_from_partial_street_name(std::string street_prefix) {
    return gData.getStreetIDsFromStreetName(street_prefix);
}

//Returns the area of the given closed feature in square meters
//Assume a non self-intersecting polygon (i.e. no holes)
//Return 0 if this feature is not a closed polygon.
double find_feature_area(int feature_id) {
    //Ensure feature is a closed polygon, else return 0
    LatLon firstPoint = getFeaturePoint(0, feature_id);
    LatLon lastPoint = getFeaturePoint(getFeaturePointCount(feature_id) - 1, feature_id);
    if (firstPoint.lat() != lastPoint.lat() || firstPoint.lon() != lastPoint.lon()) 
        return 0;
    
    //Find the area of the feature using the shoelace formula
    double area = 0;
    int numPoints = getFeaturePointCount(feature_id);
    
    int j = numPoints - 1;
    for (int i = 0; i < numPoints; ++i) {
        //Get two LatLon points from the feature
        LatLon point1 = getFeaturePoint(i, feature_id);
        LatLon point2 = getFeaturePoint(j, feature_id);
        
        //Compute the difference in x-position and y-position according to shoelace formula
        // (x[j] + x[i]) * (y[j) - y[i])
        double latAvg = DEGREE_TO_RADIAN * ((point1.lat() + point2.lat()) / 2.0);   
        double xDiff = (DEGREE_TO_RADIAN * point2.lon() * cos(latAvg)) 
                       + DEGREE_TO_RADIAN * (point1.lon() * cos(latAvg)); 
        xDiff = xDiff * EARTH_RADIUS_METERS;
        
        double yDiff = DEGREE_TO_RADIAN * (point2.lat() - point1.lat()); 
        yDiff = yDiff * EARTH_RADIUS_METERS;
        
        //Compute the signed area
        area += xDiff * yDiff;
        j = i;
    }
    return abs(area / 2.0);
}

//Returns the length of the OSMWay that has the given OSMID, in meters.
//To implement this function you will have to  access the OSMDatabaseAPI.h 
//functions.
double find_way_length(OSMID way_id) {
    double length = 0;
    std::vector<LatLon> nodePos;
    // Gets way index based on the OSMID
    const unsigned wayInd = gData.getWayIndexOfOSMID(way_id);
    // Gets way from its index
    const OSMWay* way = getWayByIndex(wayInd);
    // Gets the OSMIDs of the nodes in the way 
    std::vector<OSMID> nodeIDs = getWayMembers(way);

    // Loop through all the OSMIDs of the nodes and put their LatLons into a vector
    for (const auto& ID : nodeIDs) {
        unsigned nodeInd = gData.getNodeIndexOfOSMID(ID); // Get nodes index
        LatLon nodeLatLon = getNodeCoords(getNodeByIndex(nodeInd));
        nodePos.push_back(nodeLatLon);
    }
    if (nodePos.size() == 1)
        return 0;
    else {
        for (unsigned i = 0; i < nodePos.size() - 1; ++i) 
            length += find_distance_between_two_points(std::make_pair(nodePos[i], nodePos[i + 1]));
    }
    return length;
}
