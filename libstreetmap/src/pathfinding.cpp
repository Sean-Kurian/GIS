//==============================================================================
// File Description: Defines functions used to compute paths between 2 points
// and the time needed to travel those paths walking or with a car
//==============================================================================

#include "pathfinding.h"
#include "pathfindingHelpers.h"
#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include "globalData.h"
#include "math.h"

#include <queue>
#include <unordered_map>
#include <unordered_set>

// Computes the time needed to drive down a given path 
double compute_path_travel_time(const std::vector<StreetSegmentIndex>& path, 
                                const double turn_penalty){
    
    if (path.size() == 0) return 0; 

    double travel_time = 0; 
    InfoStreetSegment SSData, SSData2; 
    for (int i = 0; i < path.size() - 1; i++){
        SSData = getInfoStreetSegment(path[i]); 
        SSData2 = getInfoStreetSegment(path[i + 1]); 

        travel_time += find_street_segment_travel_time(path[i]); 
        if (SSData.streetID != SSData2.streetID)
            travel_time += turn_penalty; 
    }
    travel_time += find_street_segment_travel_time(path[path.size() - 1]); 
    return travel_time; 
}
// Computes the time needed to walk a given path at a given speed
double compute_path_walking_time(const std::vector<StreetSegmentIndex>& path, 
                                 const double walking_speed, 
                                 const double turn_penalty){
    
    if (path.size() == 0) return 0; 

    double travel_time = 0; 
    InfoStreetSegment SSData, SSData2; 
    for (int i = 0; i < path.size() - 1; i++) {
        SSData = getInfoStreetSegment(path[i]); 
        SSData2 = getInfoStreetSegment(path[i + 1]); 

        travel_time += find_street_segment_length(path[i]) / walking_speed; 
        if(SSData.streetID != SSData2.streetID){
            travel_time += turn_penalty; 
        }
    }
    travel_time += find_street_segment_length(path[path.size() - 1]) / walking_speed;
    return travel_time; 
}

// Uses A* algorithm to find fastest path between 2 intersections
std::vector<StreetSegmentIndex> find_path_between_intersections(
                                const IntersectionIndex intersect_id_start,
                                const IntersectionIndex intersect_id_end, 
                                const double turn_penalty) {
    // Nodes to explore next
    std::priority_queue<waveElem, std::vector<waveElem>, compare> toVisit;
    // Stores nodes that have been visited so far
    std::unordered_map<unsigned, aStarNode*> visited;
    
    aStarNode* baseNode = new aStarNode(intersect_id_start, -1, -1, std::numeric_limits<double>::max());
    visited.insert(std::make_pair(intersect_id_start, baseNode));
    
    double distToEnd = find_distance_between_two_points(std::make_pair(
                            getIntersectionPosition(intersect_id_start),
                            getIntersectionPosition(intersect_id_end)));
    int maxSpeed = gData.getMaxSpeed();
    // Est time to end is distance * 1 / 100 (1 / speed limit) * 3.6 (conversion)
    double estTotalTime = distToEnd * (1 / maxSpeed) * 3.6;
    
    waveElem baseElem(baseNode, -1, -1, 0, estTotalTime); 
    toVisit.push(baseElem);
    while (!toVisit.empty()) {
        waveElem wave = toVisit.top();  // Get next element
        toVisit.pop();                  // Remove from wavefront
        aStarNode* currNode = wave.node;
        if (currNode->intID == intersect_id_end) 
           return findPathTaken(visited, intersect_id_start, intersect_id_end, true);

        if (wave.timeToNode < currNode->bestTime) {
            currNode->bestTime = wave.timeToNode;
            currNode->parentInt = wave.parentInt;
            currNode->parentEdge = wave.parentEdge;

            std::vector<pairSegIntID> adjSegIntIDs = gData.getAdjacentSegIntIDsOfInt(currNode->intID);
            for (const pairSegIntID& segIntID : adjSegIntIDs) {
                aStarNode* toNode = getToNode(segIntID.second, visited);
                if (toNode == NULL) {
                    toNode = new aStarNode(segIntID.second, currNode->intID, 
                                           segIntID.first, 
                                           std::numeric_limits<double>::max());
                    visited.insert(std::make_pair(segIntID.second, toNode));
                }

                double timeToNode = wave.timeToNode + gData.getTravelTimeOfSeg(segIntID.first)
                                  + determineTurnPenalty(wave.parentEdge, segIntID.first, turn_penalty);
                distToEnd = find_distance_between_two_points(std::make_pair(
                                        getIntersectionPosition(segIntID.second),
                                        getIntersectionPosition(intersect_id_end)));
                // Est time to end is distance * 1 / 100 (1 / speed limit) * 3.6 (conversion)
                estTotalTime = timeToNode + (distToEnd * (1 / maxSpeed) * 3.6);
                // Add neighbour to open set
                toVisit.push(waveElem(toNode, currNode->intID, segIntID.first, timeToNode, estTotalTime));
            }
        }
    }
    // Open set empty but no path found
    std::cerr << "Open set empty but no path found. May be impossible\n"; 
    std::vector<int> failure = {};
    return failure;
}

// Uses A* to find fastest path between 2 intersections with walking to a start point
std::pair<std::vector<StreetSegmentIndex>, std::vector<StreetSegmentIndex> > 
find_path_with_walk_to_pick_up(const IntersectionIndex start_intersection, 
                              const IntersectionIndex end_intersection, 
                              const double turn_penalty, 
                              const double walking_speed, 
                              const double walking_time_limit) {
    std::vector<int> pathWalked;
    std::vector<int> pathDriven;
    // Nodes to explore next
    std::priority_queue<waveElem, std::vector<waveElem>, compare> walkToVisit;
    // Stores nodes that have been visited so far
    std::unordered_map<unsigned, aStarNode*> walkVisited;
    // Stores the further intersections that can be walked to
    std::unordered_map<double, aStarNode*> maxWalkableInts;
    
    aStarNode* baseNode = new aStarNode(start_intersection, -1, -1, std::numeric_limits<double>::max());
    walkVisited.insert(std::make_pair(start_intersection, baseNode));
    
    double distToEnd = find_distance_between_two_points(std::make_pair(
                            getIntersectionPosition(start_intersection),
                            getIntersectionPosition(end_intersection)));
    // Est time to end is distance / speed
    double estTotalTime = distToEnd / walking_speed;
    
    waveElem baseElem(baseNode, -1, -1, 0, estTotalTime); 
    walkToVisit.push(baseElem);
    while (!walkToVisit.empty()) {
        waveElem wave = walkToVisit.top();  // Get next element
        walkToVisit.pop();                  // Remove from wavefront
        aStarNode* currNode = wave.node;
        if (currNode->intID == end_intersection) {
            pathWalked = findPathTaken(walkVisited, start_intersection, end_intersection, true);
            std::vector<int> zeroVector;
            return std::make_pair(pathWalked, zeroVector);
        }
        // Found better path to the code
        if (wave.timeToNode < currNode->bestTime) {
            currNode->bestTime = wave.timeToNode;
            currNode->parentInt = wave.parentInt;
            currNode->parentEdge = wave.parentEdge;

            std::vector<int> segsOfInt = find_street_segments_of_intersection(currNode->intID);
            for (const int& segID : segsOfInt) {
                int toInt;
                InfoStreetSegment SSData = getInfoStreetSegment(segID);
                if (SSData.from == currNode->intID)
                    toInt = SSData.to;
                else
                    toInt = SSData.from;
                aStarNode* toNode = getToNode(toInt, walkVisited);
                if (toNode == NULL) {
                    toNode = new aStarNode(toInt, currNode->intID, segID, 
                                           std::numeric_limits<double>::max());
                    walkVisited.insert(std::make_pair(toInt, toNode));
                }

                double timeToNode = wave.timeToNode + (find_street_segment_length(toInt) / walking_speed)
                                  + determineTurnPenalty(wave.parentEdge, segID, turn_penalty);
                //Case for time to node greater than walking time limit
                if (timeToNode >= walking_time_limit) {
                    distToEnd = find_distance_between_two_points(std::make_pair(
                                        getIntersectionPosition(currNode->intID),
                                        getIntersectionPosition(end_intersection)));
                    estTotalTime = currNode->bestTime + (distToEnd * (1 / gData.getMaxSpeed()) * 3.6);
                    maxWalkableInts.insert(std::make_pair(estTotalTime, currNode));
                    continue;
                }

                distToEnd = find_distance_between_two_points(std::make_pair(
                                        getIntersectionPosition(toInt),
                                        getIntersectionPosition(end_intersection)));
               
                estTotalTime = timeToNode + (distToEnd / walking_speed);
                // Add neighbour to open set
                walkToVisit.push(waveElem(toNode, currNode->intID, segID, timeToNode, estTotalTime));
                maxWalkableInts.insert(std::make_pair(estTotalTime, currNode));
            }
        }
    }
    // Map which stores the optimal full paths
    std::map<double, std::pair<std::vector<int>, std::vector<int>>> totalPaths;

    // Goes over max intersections reachable by walking and finds the driving paths from there
    for (const auto& maxInt : maxWalkableInts) {
        pathWalked = findPathTaken(walkVisited, start_intersection, maxInt.second->intID, false);
        double timeWalked = compute_path_walking_time(pathWalked, walking_speed, turn_penalty);
        pathDriven = find_path_between_intersections(maxInt.second->intID, end_intersection, turn_penalty);
        double timeDrove = compute_path_travel_time(pathDriven, turn_penalty);
        double totalTime = timeWalked + timeDrove;
        totalPaths.insert(std::make_pair(totalTime, std::make_pair(pathWalked, pathDriven)));
    }
    for (auto& mapElem : walkVisited)
        delete mapElem.second;
    return totalPaths.begin()->second;
}
