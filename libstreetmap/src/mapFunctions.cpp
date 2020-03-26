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
// File Description: This file implements functions which are needed to perform
// essential operations on map data for a GIS application
// For example: finding distance between two points
//==============================================================================

#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "globalData.h"
#include "math.h"


#include <set>
#include <algorithm>

// Returns the distance between two coordinates in meters
double find_distance_between_two_points(std::pair<LatLon, LatLon> points) {
    // Calculates the average latitude between the two points
    double latAvg = DEGREE_TO_RADIAN * ((points.first.lat() + points.second.lat()) * 0.5);
    // Finds the differences in x and y coordinates
    double xDiff = (DEGREE_TO_RADIAN * points.second.lon() * cos(latAvg)) 
                   - DEGREE_TO_RADIAN * (points.first.lon() * cos(latAvg)); 
    double yDiff = DEGREE_TO_RADIAN * (points.second.lat() - points.first.lat()); 
    // Returns distance using Pythagoras's Theorem
    return (EARTH_RADIUS_METERS * sqrt(xDiff*xDiff + yDiff*yDiff)); 
}

// Returns the length of the given street segment in meters
double find_street_segment_length(int street_segment_id) {
    return gData.getLengthOfSegment(street_segment_id);
}



// Returns the travel time to drive a street segment in seconds 
double find_street_segment_travel_time(int street_segment_id) {
    return gData.getTravelTimeOfSeg(street_segment_id);
}

// Returns the nearest intersection to the given position by checking distance to
// all intersections and finding the shortest
int find_closest_intersection(LatLon my_position) {
    //Start by assuming that the 0th intersection is the closest
    int closestIntersection = 0;
    double closestDistance = find_distance_between_two_points(std::make_pair(my_position, 
                                                              getIntersectionPosition(0)));
    //Check each intersection against the 0th intersection to see if it's closer
    for (int intersectionID = 1; intersectionID < getNumIntersections(); ++intersectionID) {
        // Get position of intersection and find distance
        LatLon intersectionPos = getIntersectionPosition(intersectionID);
        double distance = find_distance_between_two_points(std::make_pair(my_position, intersectionPos));
        // If its closer change data to this intersection being closest
        if (distance < closestDistance) {
            closestDistance = distance;
            closestIntersection = intersectionID;
        }
    }
    return closestIntersection;
}

// Returns the street segments for the given intersection 
std::vector<int> find_street_segments_of_intersection(int intersection_id) {
    return gData.getSegsOfIntersection(intersection_id);
}

// Returns street names at the given intersection (includes duplicate street names) 
std::vector<std::string> find_street_names_of_intersection(int intersection_id) {
    std::vector<std::string> streetNames;
    InfoStreetSegment SSData;
    // Gets all segments of the intersection
    std::vector<int> segsOfInt = gData.getSegsOfIntersection(intersection_id);
    
    // Loops through the segments and adds their names to return vector
    for (const int& segment : segsOfInt) {
        SSData = getInfoStreetSegment(segment);
        streetNames.push_back(getStreetName(SSData.streetID));
    }
    return streetNames;
}

// Returns true if you can get from intersection_ids.first to intersection_ids.second 
// using a single street segment (hint: check for 1-way streets too)
bool are_directly_connected(std::pair<int, int> intersection_ids) {
    // Checks if intersection is connected to itself
    if (intersection_ids.first == intersection_ids.second) 
        return true; 
    // Finds adjacent intersections which are reachable from intersection 1
    std::vector<int> adjacentIntersections = find_adjacent_intersections(intersection_ids.first);
    // Loops through adjacent intersections looking for intersection 2
    for (const int& intersection : adjacentIntersections) {
        if (intersection == intersection_ids.second) 
            return true; 
    }
    return false; 
}

// Returns all intersections reachable by traveling down one street segment 
// from given intersection. Does not contain duplicate intersections
std::vector<int> find_adjacent_intersections(int intersection_id) {
    std::vector<int> adjacentInts;
    std::vector<pairSegIntID> adjSegIntIDs = gData.getAdjacentSegIntIDsOfInt(intersection_id);
    for (const pairSegIntID& adjIDs : adjSegIntIDs) 
        adjacentInts.push_back(adjIDs.second);
    return adjacentInts;
}

// Returns all street segments for the given street
std::vector<int> find_street_segments_of_street(int street_id) {
    return gData.getSegsOfStreet(street_id);
}

// Returns all intersections along the a given street
std::vector<int> find_intersections_of_street(int street_id) {
    return gData.getIntersectionsOfStreet(street_id);
}

// Return all intersection ids for two intersecting streets
std::vector<int> find_intersections_of_two_streets(std::pair<int, int> street_ids) {
    // Get all intersections of the two streets in sorted vector
    std::vector<int> streetOne = gData.getIntersectionsOfStreet(street_ids.first); 
    std::vector<int> streetTwo = gData.getIntersectionsOfStreet(street_ids.second); 
    // Vector to store result
    std::vector<int> intersections; 
    // Find mutual elements and store in vector
    std::set_intersection(streetOne.begin(), streetOne.end(), 
                          streetTwo.begin(), streetTwo.end(), 
                          std::back_inserter(intersections)); 
    return intersections;
}

// Returns all streetIDs corresponding to street names that start with a given prefix
// Function is case-insensitive to the street prefix and ignores spaces.
// For example, both "dun " and "dun d a s" are prefixes to "Dundas Street".
// If no street names match the given prefix the function returns an empty vector
std::vector<int> find_street_ids_from_partial_street_name(std::string street_prefix) {
    return gData.getStreetIDsFromStreetName(street_prefix);
}

//Returns the area of the given closed feature in square meters
//Returns 0 if the feature is not a closed polygon.
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
    // Returns area using shoelace formula
    return abs(area / 2.0);
}

//Returns the length of the OSMWay that has the given OSMID, in meters.
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
    // Checks if singular point
    if (nodePos.size() == 1)
        return 0;
    // Finds length of way by finding distance between one node and the next
    else {
        for (unsigned i = 0; i < nodePos.size() - 1; ++i) 
            length += find_distance_between_two_points(std::make_pair(nodePos[i], nodePos[i + 1]));
    }
    return length;
}
