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
#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include "globalData.h"
#include "math.h"

#include <set>
#include <algorithm>

MapData gData;

void getSegmentData(const unsigned& numStreetSegments);
void getIntersectionData(const unsigned& numIntersections);
void getStreetData(const unsigned& numStreets);

bool load_map(std::string mapPath) {
    bool loadSuccessful = loadStreetsDatabaseBIN(mapPath);
    
    if (loadSuccessful) {
        const unsigned numStreets = getNumStreets();
        const unsigned numSegments = getNumStreetSegments();
        const unsigned numIntersections = getNumIntersections();
        
        gData.allocStreetVecs(numStreets);
        gData.allocSegmmentVecs(numSegments);
        gData.allocIntersectionVecs(numIntersections);

        getStreetData(numStreets);
        getSegmentData(numSegments);
        getIntersectionData(numIntersections);
    }
    return loadSuccessful;
}

void close_map() {
    //Clean-up your map related data structures here
    closeStreetDatabase();
}

void getStreetData(const unsigned& numStreets) {
    for (unsigned streetInd = 0; streetInd < numStreets; ++streetInd) {
        gData.addStreetIDtoName(streetInd, getStreetName(streetInd));
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

//Returns the distance between two coordinates in meters
double find_distance_between_two_points(std::pair<LatLon, LatLon> points) {
    
    double latAvg = DEGREE_TO_RADIAN * ((points.first.lat() + points.second.lat()) / 2.0);   
    double xDiff = (DEGREE_TO_RADIAN * points.second.lon() * cos(latAvg)) - DEGREE_TO_RADIAN * (points.first.lon() * cos(latAvg)); 
    double yDiff = DEGREE_TO_RADIAN * (points.second.lat() - points.first.lat()); 
    
    return (EARTH_RADIUS_METERS * sqrt(xDiff*xDiff + yDiff*yDiff)); 
}

//Returns the length of the given street segment in meters
double find_street_segment_length(int street_segment_id) {
    InfoStreetSegment seg = getInfoStreetSegment(street_segment_id); 
    double dist = 0; 
   
    if (seg.curvePointCount == 0){
        return find_distance_between_two_points(std::make_pair(getIntersectionPosition(seg.from), getIntersectionPosition(seg.to))); 
    }
    
    dist = dist + find_distance_between_two_points(std::make_pair(getIntersectionPosition(seg.from), getStreetSegmentCurvePoint(street_segment_id,0)));
    
    for (unsigned i = 0; i < seg.curvePointCount-1; i++){
        dist = dist + find_distance_between_two_points(std::make_pair(getStreetSegmentCurvePoint(street_segment_id,i), getStreetSegmentCurvePoint(street_segment_id,i+1)));
    }
    
    dist = dist + find_distance_between_two_points(std::make_pair(getStreetSegmentCurvePoint(street_segment_id, seg.curvePointCount - 1), getIntersectionPosition(seg.to))); 
    return dist; 
}

//Returns the travel time to drive a street segment in seconds 
//(time = distance/speed_limit)
double find_street_segment_travel_time(int street_segment_id) {
    return 0;
}

//Returns the nearest intersection to the given position
int find_closest_intersection(LatLon my_position) {
    return 0;
}

//Returns the street segments for the given intersection 
std::vector<int> find_street_segments_of_intersection(int intersection_id) {
    return gData.getSegsOfIntersection(intersection_id);
}

//Returns the street names at the given intersection (includes duplicate street 
//names in returned vector)
std::vector<std::string> find_street_names_of_intersection(int intersection_id) {
    return {};
}

//Returns true if you can get from intersection_ids.first to intersection_ids.second using a single 
//street segment (hint: check for 1-way streets too)
//corner case: an intersection is considered to be connected to itself
bool are_directly_connected(std::pair<int, int> intersection_ids) {
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
    //Start by creating a set so segments are only added once
    std::set<int> streetSegmentsOfStreetSet;
    
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
                streetSegmentsOfStreetSet.insert(seg_id);
            }
        }
    }
    
    //Copy data over to a vector
    std::vector<int> streetSegmentsOfStreet(streetSegmentsOfStreetSet.begin(), streetSegmentsOfStreetSet.end());
    return streetSegmentsOfStreet;
}

//Returns all intersections along the a given street
std::vector<int> find_intersections_of_street(int street_id) {
    return gData.getIntersectionsOfStreet(street_id);
}

//Return all intersection ids for two intersecting streets
//This function will typically return one intersection id.
std::vector<int> find_intersections_of_two_streets(std::pair<int, int> street_ids) {
    return {};
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
    return 0;
}

//Returns the length of the OSMWay that has the given OSMID, in meters.
//To implement this function you will have to  access the OSMDatabaseAPI.h 
//functions.
double find_way_length(OSMID way_id) {
    return 0;
}