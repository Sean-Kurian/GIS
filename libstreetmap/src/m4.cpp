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
    
    const unsigned NUM_TO_COMPLETE = deliveries.size();
    const size_t NUM_NEIGHBORS_TO_FIND = 3;
    
//    auto timeKDTree = std::chrono::nanoseconds(0);
    
    PointVec pointData;
    pointData.points.resize(NUM_TO_COMPLETE);
    for (unsigned orderNum = 0; orderNum < NUM_TO_COMPLETE; ++orderNum) {
        LatLon orderPos = getIntersectionPosition(deliveries[orderNum].pickUp);
        pointData.points[orderNum].x = xFromLon(orderPos.lon());//orderPos.lon();//xFromLon(orderPos.lon());
        pointData.points[orderNum].y = yFromLat(orderPos.lat());//orderPos.lat();//yFromLat(orderPos.lat());
    }
    
    typedef KDTreeSingleIndexDynamicAdaptor<L2_Simple_Adaptor<double, PointVec>,
                                            PointVec, 2> KDTreeType;
    
    KDTreeType index(2, pointData, KDTreeSingleIndexAdaptorParams(5));
    index.addPoints(0, NUM_TO_COMPLETE - 1);
    
    size_t resIndexes[NUM_NEIGHBORS_TO_FIND];
    double resDists[NUM_NEIGHBORS_TO_FIND];
    KNNResultSet<double> resultSet(NUM_NEIGHBORS_TO_FIND);
    
    //Determine number of deliveries needed to be completed
    unsigned currentInt = depots[0];
    unsigned numCompleted = 0;
    unsigned numPickuped = 0;
    Truck truck(truck_capacity);
    
    bool prevPathIsPickUp = true;
    unsigned pickUpOrderNum = 0;
    
//    std::cout << "Number of packages: " << NUM_TO_COMPLETE << "\n";
    
//    auto startKD = std::chrono::high_resolution_clock::now();
    
//    auto endKD = std::chrono::high_resolution_clock::now();
//    std::cout << "Time taken to make KD tree: " 
//              << std::chrono::duration_cast<std::chrono::microseconds>(endKD - startKD).count() << " us\n";
    
    //Determine the closest pickup location from current intersection
    //Determine starting depot - depot that is closest to a start point
    
//    size_t depotResIdx[1];
//    double depotResDists[1];
//    KNNResultSet<double> depotResultSet(1);
//    
//    for (const int& depot : depots) {
//        LatLon currentPos = getIntersectionPosition(depot);
//        const double queryPoint[2] = {xFromLon(currentPos.lon()), yFromLat(currentPos.lat())};
//        depotResultSet.init(depotResIdx, depotResDists);
//        index.findNeighbors(depotResultSet, queryPoint, nanoflann::SearchParams(5));
//        double distToOrder = find_distance_between_two_points(std::make_pair(
//                                     getIntersectionPosition(depot),
//                                     getIntersectionPosition(deliveries[depotResIdx[0]].pickUp)));
//    }
//    
    double closestDistanceToDepot = std::numeric_limits<double>::max();
    unsigned closestDepot = depots[0];
    unsigned closestOrderFromDepot = 0;
    for (unsigned depotNum = 0; depotNum < depots.size(); ++depotNum) {
        for (unsigned orderNum = 0; orderNum < NUM_TO_COMPLETE; ++orderNum) {
            double distToOrder = find_distance_between_two_points(std::make_pair(
                                     getIntersectionPosition(depots[depotNum]),
                                     getIntersectionPosition(deliveries[orderNum].pickUp)));

            if (distToOrder < closestDistanceToDepot) {
                closestDistanceToDepot = distToOrder;
                closestOrderFromDepot = orderNum;
                closestDepot = depotNum;
            }
        }
    }

    //Construct the first sub path
    CourierSubpath initialPath;
    currentInt = depots[closestDepot];
    initialPath.start_intersection = currentInt;
    initialPath.end_intersection = deliveries[closestOrderFromDepot].pickUp;
    initialPath.subpath = find_path_between_intersections(initialPath.start_intersection, initialPath.end_intersection, turn_penalty);
    pickUpOrderNum = closestOrderFromDepot;
    result.push_back(initialPath);
    currentInt = initialPath.end_intersection;
    bool isFirstPath = true;
    
    index.removePoint(pickUpOrderNum);
    
    while (numCompleted < NUM_TO_COMPLETE) {
        CourierSubpath toPickup, toDropoff, nextPath;

        if (isFirstPath) {
            nextPath.pickUp_indices.push_back(pickUpOrderNum);
            truck.addPackage(pickUpOrderNum, deliveries[pickUpOrderNum].itemWeight);
            numPickuped++;
            isFirstPath = false;
            prevPathIsPickUp = false;
        }

        bool isPickup = true;
        int closestOrder = -1;
        double closestDistance = std::numeric_limits<double>::max();
        
//        auto startFindKD = std::chrono::high_resolution_clock::now();
        
        if (numPickuped < NUM_TO_COMPLETE) {
            LatLon currentPos = getIntersectionPosition(currentInt);
            const double queryPoint[2] = {xFromLon(currentPos.lon()), yFromLat(currentPos.lat())};//{(currentPos.lon()), (currentPos.lat())};
            resultSet.init(resIndexes, resDists);
            index.findNeighbors(resultSet, queryPoint, nanoflann::SearchParams(5));

            for (unsigned neighbor = 0; neighbor < NUM_NEIGHBORS_TO_FIND; ++neighbor) {
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
        
        if (closestOrder == -1) {
            std::cout << "\nWeight: " << truck.curWeight << " Capacity: " << truck.capacity << " Num Packages: " << truck.packages.size()
                      << " NumPickuped: " << numPickuped << " NumCompleted: " << numCompleted << " Num to Complete: " << NUM_TO_COMPLETE << "\nNeighbors: ";
            for (unsigned neighbor = 0; neighbor < NUM_NEIGHBORS_TO_FIND; ++neighbor)
                std:: cout << resIndexes[neighbor] << "  ";
            std::cout << "\n\n";
        }
        
//        auto endFindKD = std::chrono::high_resolution_clock::now();
//        timeKDTree += std::chrono::duration_cast<std::chrono::nanoseconds>(endFindKD - startFindKD);
//        std::cout << "Time taken to find nearest pickup with KD tree: " 
//                  << std::chrono::duration_cast<std::chrono::nanoseconds>(endFindKD - startFindKD).count() << " ns\n";
        std::cout << "Closest index with KD tree: " << closestOrder << " Distance: " << closestDistance
                  << " Pickup: " << (isPickup ? "Yes" : "No")
                  << " Current weight: " << truck.curWeight << " Package size: " << deliveries[closestOrder].itemWeight << "\n";

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
            index.removePoint(pickUpOrderNum);
            numPickuped++;
        }
        else {
            nextPath.end_intersection = deliveries[closestOrder].dropOff;
            truck.removePackage(closestOrder, deliveries[closestOrder].itemWeight);
            numCompleted++;
        }
        nextPath.subpath = find_path_between_intersections(nextPath.start_intersection, nextPath.end_intersection, turn_penalty);
        result.push_back(nextPath);

        currentInt = nextPath.end_intersection;
    }

    //Last stop to Depot
    CourierSubpath toDepot;
    toDepot.start_intersection = currentInt;

    //Determine closest depot to go to
    double closestDepotDistance = std::numeric_limits<double>::max();
    closestDepot = 0;
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
    result.push_back(toDepot);
    
    std::cout << "Truck at end: Num Packages: " << truck.packages.size() << " Weight: " << truck.curWeight << "\n";
    
    return result;
}







std::vector<CourierSubpath> v2traveling_courier(const std::vector<DeliveryInfo>& deliveries,
                                              const std::vector<int>& depots,
                                              const float turn_penalty,
                                              const float truck_capacity) {
    std::vector<CourierSubpath> result;
    
    //Determine number of deliveries needed to be completed
    //unsigned randDepot = std::rand() % depots.size();
    unsigned currentInt = depots[0];
    std::vector<bool> orderComplete;
    orderComplete.resize(deliveries.size(), false);
    const unsigned NUM_TO_COMPLETE = deliveries.size();
    unsigned numCompleted = 0;
    Truck truck(truck_capacity);
    bool prevPathIsPickUp = true;
    unsigned pickUpOrderNum = 0;
        
    //Determine the closest pickup location from current intersection
    //Determine starting depot - depot that is closest to a start point
    double closestDistanceToDepot = std::numeric_limits<double>::max();
    unsigned closestDepot = depots[0];
    unsigned closestOrderFromDepot = 0;
    for (unsigned depotNum = 0; depotNum < depots.size(); ++depotNum) {
        for (unsigned orderNum = 0; orderNum < NUM_TO_COMPLETE; ++orderNum) {
            double distToOrder = find_distance_between_two_points(std::make_pair(
                                     getIntersectionPosition(depots[depotNum]),
                                     getIntersectionPosition(deliveries[orderNum].pickUp)));

            if (distToOrder < closestDistanceToDepot) {
                closestDistanceToDepot = distToOrder;
                closestOrderFromDepot = orderNum;
                closestDepot = depotNum;
            }
        }
    }

    //Construct the first sub path
    CourierSubpath initialPath;
    currentInt = depots[closestDepot];
    initialPath.start_intersection = currentInt;
    initialPath.end_intersection = deliveries[closestOrderFromDepot].pickUp;
    initialPath.subpath = find_path_between_intersections(initialPath.start_intersection, initialPath.end_intersection, turn_penalty);
    pickUpOrderNum = closestOrderFromDepot;
    result.push_back(initialPath);
    currentInt = initialPath.end_intersection;
    bool isFirstPath = true;
    
    while (numCompleted < NUM_TO_COMPLETE) {
        CourierSubpath toPickup, toDropoff, nextPath;

        if (isFirstPath) {
            nextPath.pickUp_indices.push_back(pickUpOrderNum);
            truck.addPackage(pickUpOrderNum, deliveries[pickUpOrderNum].itemWeight);
            isFirstPath = false;
            prevPathIsPickUp = false;
        }

        //Determine the closest pickup or drop off location from current intersection
        double closestDistance = std::numeric_limits<double>::max();
        unsigned closestOrder = 0;
        bool isPickUp = true;
        
        auto startFind = std::chrono::high_resolution_clock::now();
        for (unsigned orderNum = 0; orderNum < NUM_TO_COMPLETE; ++orderNum) {
            double distToOrder = std::numeric_limits<double>::max();
            bool pickUp = false;
            if (!orderComplete[orderNum]) {
                //Determine if order is not on truck, then find it's pickup distance
                if (!truck.packages.count(orderNum)) {
                    //First ensure there is room on the truck to possibly pick up the package
                    if (truck.curWeight + deliveries[orderNum].itemWeight < truck.capacity) {
                        distToOrder = find_distance_between_two_points(std::make_pair(
                                     getIntersectionPosition(currentInt),
                                     getIntersectionPosition(deliveries[orderNum].pickUp)));
                        pickUp = true;
                    }
                }
                // Otherwise order is on truck, so find it's drop off distance
                else {
                    distToOrder = find_distance_between_two_points(std::make_pair(
                                     getIntersectionPosition(currentInt),
                                     getIntersectionPosition(deliveries[orderNum].dropOff)));
                    pickUp = false;
                }

                if (distToOrder < closestDistance) {
                    closestDistance = distToOrder;
                    closestOrder = orderNum;
                    isPickUp = pickUp;
                }
            }
        }
//        auto endFind = std::chrono::high_resolution_clock::now();
//        std::cout << "Time taken to brute force nearest pickup: " 
//                  << std::chrono::duration_cast<std::chrono::nanoseconds>(endFind - startFind).count() << " ns\n";

        std::cout << "Closest index with brute force: " << closestOrder << "\n";

        //If the last path was to a pick-up, the current path is from the pick up, so pick up the package
        if (prevPathIsPickUp) {
            nextPath.pickUp_indices.push_back(pickUpOrderNum);
            truck.addPackage(pickUpOrderNum, deliveries[pickUpOrderNum].itemWeight);
            prevPathIsPickUp = false;
        }

        //Construct the courier sub-path from current intersection to next location
        nextPath.start_intersection = currentInt;
        if (isPickUp) {
            nextPath.end_intersection = deliveries[closestOrder].pickUp;
            prevPathIsPickUp = true;
            pickUpOrderNum = closestOrder;
        } else {
            nextPath.end_intersection = deliveries[closestOrder].dropOff;
            truck.removePackage(closestOrder, deliveries[closestOrder].itemWeight);
            orderComplete[closestOrder] = true;
            numCompleted++;
        }

        nextPath.subpath = find_path_between_intersections(nextPath.start_intersection, nextPath.end_intersection, turn_penalty);
        result.push_back(nextPath);
        currentInt = nextPath.end_intersection;
    }
    
    //Last stop to Depot
    CourierSubpath toDepot;
    toDepot.start_intersection = currentInt;

    //Determine closest depot to go to
    double closestDepotDistance = std::numeric_limits<double>::max();
    closestDepot = 0;
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
    result.push_back(toDepot);
    return result;
}