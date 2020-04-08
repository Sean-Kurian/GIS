#include "StreetsDatabaseAPI.h"
#include "m1.h"
#include "m4.h"
#include "m4Helpers.h"
#include "pathfinding.h"
#include "drawMapHelpers.h"

// Nanoflann is an open-source library for KD-trees
// Found at https://github.com/jlblancoc/nanoflann 
#include <nanoflann.hpp>

#include <limits>
#include <cstdlib>
#include <map>
#include <unordered_set>
#include <stdio.h>
#include <chrono>
#include <bits/unordered_set.h>

using namespace nanoflann;

struct PointVec {
    struct point {
        double x, y;
    };
    std::vector<point> points;
    
    inline size_t kdtree_get_point_count() const {
        return points.size();
    }
    inline double kdtree_get_pt(const size_t idx, const size_t dim) const {
        if (dim == 0)
            return points[idx].x;
        else
            return points[idx].y;
    }
    template <class BBOX>
    bool kdtree_get_bbox(BBOX&) const {
        return false;
    }
};

std::vector<CourierSubpath> traveling_courier(const std::vector<DeliveryInfo>& deliveries,
                                              const std::vector<int>& depots,
                                              const float turn_penalty,
                                              const float truck_capacity) {
    std::vector<CourierSubpath> result;
    
    const unsigned NUM_STARTS = 3;
    const unsigned NUM_TO_COMPLETE = deliveries.size();
    const size_t NUM_NEIGHBORS_TO_FIND = 3;
    
//    std::cout << "Number of packages: " << NUM_TO_COMPLETE << "\n";

    PointVec pointData;
    pointData.points.resize(NUM_TO_COMPLETE);
    for (unsigned orderNum = 0; orderNum < NUM_TO_COMPLETE; ++orderNum) {
        LatLon orderPos = getIntersectionPosition(deliveries[orderNum].pickUp);
        pointData.points[orderNum].x = xFromLon(orderPos.lon());
        pointData.points[orderNum].y = yFromLat(orderPos.lat());
    }
    typedef KDTreeSingleIndexDynamicAdaptor<L2_Simple_Adaptor<double, PointVec>,
                                            PointVec, 2> KDTreeType;
    KDTreeType mainIndex(2, pointData, KDTreeSingleIndexAdaptorParams(5));
    mainIndex.addPoints(0, NUM_TO_COMPLETE - 1);

    //Determine the closest pickup location from every depot
    size_t depotResIdx[1];
    double depotResDists[1];
    KNNResultSet<double> depotResultSet(1);
    std::map<double, std::pair<int, int>> depotsByDistToStart;
    for (const int& depot : depots) {
        LatLon currentPos = getIntersectionPosition(depot);
        const double queryPoint[2] = {xFromLon(currentPos.lon()), yFromLat(currentPos.lat())};
        depotResultSet.init(depotResIdx, depotResDists);
        mainIndex.findNeighbors(depotResultSet, queryPoint, nanoflann::SearchParams(5));
        double distToOrder = find_distance_between_two_points(std::make_pair(
                                     getIntersectionPosition(depot),
                                     getIntersectionPosition(deliveries[depotResIdx[0]].pickUp)));
        depotsByDistToStart.insert(std::make_pair(distToOrder, std::make_pair(depot, depotResIdx[0])));
    }
    
    ///////////////////////////////////////////// THREAD STARTS /////////////////////////////////////////////
    
    for (unsigned startNum = 0; startNum < NUM_STARTS; ++startNum) {
        std::vector<CourierSubpath> threadBest;
        
        KDTreeType threadIndex(2, pointData, KDTreeSingleIndexAdaptorParams(5));
        threadIndex.addPoints(0, NUM_TO_COMPLETE - 1);
        size_t resIndexes[NUM_NEIGHBORS_TO_FIND];
        double resDists[NUM_NEIGHBORS_TO_FIND];
        KNNResultSet<double> resultSet(NUM_NEIGHBORS_TO_FIND);
        
        //Determine number of deliveries needed to be completed
        unsigned numCompleted = 0, numPickedup = 0;
        Truck truck(truck_capacity);
        
        //Construct the first sub path
        CourierSubpath initialPath;
        unsigned currentInt = depotsByDistToStart.begin()->second.first;
        unsigned pickUpOrderNum = depotsByDistToStart.begin()->second.second;
        initialPath.start_intersection = currentInt;
        initialPath.end_intersection = deliveries[pickUpOrderNum].pickUp;
        initialPath.subpath = find_path_between_intersections(initialPath.start_intersection, initialPath.end_intersection, turn_penalty);
        threadBest.push_back(initialPath);
        currentInt = initialPath.end_intersection;
        bool isFirstPath = true, prevPathIsPickUp = true;

        while (numCompleted < NUM_TO_COMPLETE) {
            CourierSubpath toPickup, toDropoff, nextPath;

            if (isFirstPath) {
                nextPath.pickUp_indices.push_back(pickUpOrderNum);
                truck.addPackage(pickUpOrderNum, deliveries[pickUpOrderNum].itemWeight);
                threadIndex.removePoint(pickUpOrderNum);
                numPickedup++;
                isFirstPath = false;
                prevPathIsPickUp = false;
            }

            bool isPickup = true;
            int closestOrder = -1;
            double closestDistance = std::numeric_limits<double>::max();

            if (numPickedup < NUM_TO_COMPLETE) {
                LatLon currentPos = getIntersectionPosition(currentInt);
                const double queryPoint[2] = {xFromLon(currentPos.lon()), yFromLat(currentPos.lat())};
                resultSet.init(resIndexes, resDists);
                threadIndex.findNeighbors(resultSet, queryPoint, nanoflann::SearchParams(5));
                unsigned minNeighbor = std::min((unsigned)NUM_NEIGHBORS_TO_FIND, (NUM_TO_COMPLETE - numPickedup));
                for (unsigned neighbor = 0; neighbor < minNeighbor; ++neighbor) {
                    if (truck.curWeight + deliveries[resIndexes[neighbor]].itemWeight < truck.capacity) {
                        closestOrder = resIndexes[neighbor];
                        closestDistance = find_distance_between_two_points(std::make_pair(
                                               getIntersectionPosition(currentInt),
                                               getIntersectionPosition(deliveries[closestOrder].pickUp)));
                        break;
                    }
                }
            }
            for (const unsigned& orderNum : truck.packages) {
                double distToOrder = find_distance_between_two_points(std::make_pair(
                                        getIntersectionPosition(currentInt),
                                        getIntersectionPosition(deliveries[orderNum].dropOff)));
                if (distToOrder < closestDistance) {
                    closestDistance = distToOrder;
                    closestOrder = orderNum;
                    isPickup = false;
                }
            }

    //        auto endFindKD = std::chrono::high_resolution_clock::now();
    //        timeKDTree += std::chrono::duration_cast<std::chrono::nanoseconds>(endFindKD - startFindKD);
    //        std::cout << "Time taken to find nearest pickup with KD tree: " 
    //                  << std::chrono::duration_cast<std::chrono::nanoseconds>(endFindKD - startFindKD).count() << " ns\n";
    //        std::cout << "Closest index with KD tree: " << closestOrder << " Distance: " << closestDistance
    //                  << " Pickup: " << (isPickup ? "Yes" : "No")
    //                  << " Current weight: " << truck.curWeight << " Package size: " << deliveries[closestOrder].itemWeight << "\n";

            //If the last path was to a pick-up, the current path is from the pick up, so pick up the package
            if (prevPathIsPickUp) {
                nextPath.pickUp_indices.push_back(pickUpOrderNum);
                prevPathIsPickUp = false;
            }

            //Construct the courier sub-path from current intersection to next location
            nextPath.start_intersection = currentInt;
            if (isPickup) {
                nextPath.end_intersection = deliveries[closestOrder].pickUp;
                prevPathIsPickUp = true;
                pickUpOrderNum = closestOrder;
                truck.addPackage(pickUpOrderNum, deliveries[pickUpOrderNum].itemWeight);
                threadIndex.removePoint(pickUpOrderNum);
                numPickedup++;
            }
            else {
                nextPath.end_intersection = deliveries[closestOrder].dropOff;
                truck.removePackage(closestOrder, deliveries[closestOrder].itemWeight);
                numCompleted++;
            }
            nextPath.subpath = find_path_between_intersections(nextPath.start_intersection, nextPath.end_intersection, turn_penalty);
            threadBest.push_back(nextPath);

            currentInt = nextPath.end_intersection;
        }

        //Last stop to Depot
        CourierSubpath toDepot;
        toDepot.start_intersection = currentInt;

        //Determine closest depot to go to
        double closestDepotDistance = std::numeric_limits<double>::max();
        unsigned closestDepot = 0;
        for (unsigned depotNum = 0; depotNum < depots.size(); ++depotNum) {
            double distToDepot = find_distance_between_two_points(std::make_pair(
                                         getIntersectionPosition(currentInt),
                                         getIntersectionPosition(depots[depotNum])));
            if (distToDepot < closestDepotDistance) {
                closestDepotDistance = distToDepot;
                closestDepot = depotNum;
            }
        }
        toDepot.end_intersection = depots[closestDepot];
        toDepot.subpath = find_path_between_intersections(currentInt,
                                                          depots[closestDepot],
                                                          turn_penalty);
        threadBest.push_back(toDepot);
    }
//    std::cout << "Truck at end: Num Packages: " << truck.packages.size() << " Weight: " << truck.curWeight << "\n";
    
    return result;
}
