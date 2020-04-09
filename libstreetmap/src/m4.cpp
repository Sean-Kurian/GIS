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
#include <omp.h>

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

std::vector<CourierSubpath> findBasePath(const std::vector<DeliveryInfo>& deliveries,
                                         const std::vector<int>& depots,
                                         const float turn_penalty,
                                         const float truck_capacity,
                                         const PointVec& pointData,
                                         const CourierSubpath& initialPath,
                                         const unsigned initialOrderNum);

//
std::vector<CourierSubpath> findRandomisedPath(const std::vector<DeliveryInfo>& deliveries,
                                               const std::vector<int>& depots,
                                               const float turn_penalty,
                                               const float truck_capacity,
                                               const PointVec& pointData,
                                               const CourierSubpath& initialPath,
                                               const unsigned initialOrderNum);



std::vector<CourierSubpath> traveling_courier(const std::vector<DeliveryInfo>& deliveries,
                                              const std::vector<int>& depots,
                                              const float turn_penalty,
                                              const float truck_capacity) {
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<CourierSubpath> result;
    double timeOfResult = std::numeric_limits<double>::max();

    const unsigned NUM_TO_COMPLETE = deliveries.size();
    
    std::cout << "Number of packages: " << NUM_TO_COMPLETE << " Number of depots: " << depots.size() << "\n";

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
#pragma omp parallel
{
    size_t count = 0;
    int threadID = omp_get_thread_num();
    int numThreads = omp_get_num_threads();
    for (auto mapItr = depotsByDistToStart.begin(); mapItr != depotsByDistToStart.end(); ++mapItr, ++count) {
        if (count % numThreads != threadID)
            continue;
        std::vector<CourierSubpath> threadBest;
        double threadBestTime;
        
//        std::cout << " Thread " << threadID << " Going to " << count << "\n";
        
        CourierSubpath initialPath;
        unsigned initialOrderNum = mapItr->second.second;
        initialPath.start_intersection = mapItr->second.first;
        initialPath.end_intersection = deliveries[initialOrderNum].pickUp;
        initialPath.subpath = find_path_between_intersections(initialPath.start_intersection, initialPath.end_intersection, turn_penalty);
        
        threadBest = findBasePath(deliveries, depots, turn_penalty, truck_capacity, 
                                  pointData, initialPath, initialOrderNum);
        threadBestTime = findTotalPathTime(threadBest, turn_penalty);
        
        auto breakTime = startTime + std::chrono::milliseconds(30000);
        
        ////////////////////////////////////// RANDOM STARTS //////////////////////////////////////
        
        while (std::chrono::high_resolution_clock::now() < breakTime) {
            std::vector<CourierSubpath> threadTry = findRandomisedPath(deliveries, depots, turn_penalty, truck_capacity, 
                                                                       pointData, initialPath, initialOrderNum);
            double tryTime = findTotalPathTime(threadTry, turn_penalty);
//            std::cout << "Try time: " << tryTime << " Thread " << threadID << " best time: " << threadBestTime << "\n";
            if (findTotalPathTime(threadTry, turn_penalty) < threadBestTime) {
                std::cout << "Random time faster than base!\n";
                threadBest = threadTry;
                threadBestTime = tryTime;
            }
        }
        //Determine if threadBest is better than result
#pragma omp critical
        {
            std::cout << "Thread " << threadID << " best: " << threadBestTime << " Best time: " << timeOfResult << "\n";
            if (threadBestTime < timeOfResult) {
                timeOfResult = threadBestTime;
                result = threadBest;
            }
        }
    }
}
    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Total time to find result: " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " ms\n";
    
    return result;
}

//
std::vector<CourierSubpath> findBasePath(const std::vector<DeliveryInfo>& deliveries,
                                         const std::vector<int>& depots,
                                         const float turn_penalty,
                                         const float truck_capacity,
                                         const PointVec& pointData,
                                         const CourierSubpath& initialPath,
                                         const unsigned initialOrderNum) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<CourierSubpath> threadCurrent;
    
    const unsigned NUM_TO_COMPLETE = deliveries.size();
    const size_t NUM_NEIGHBORS_TO_FIND = 3;
    
    typedef KDTreeSingleIndexDynamicAdaptor<L2_Simple_Adaptor<double, PointVec>,
                                            PointVec, 2> KDTreeType;
    KDTreeType threadIndex(2, pointData, KDTreeSingleIndexAdaptorParams(5));
    threadIndex.addPoints(0, NUM_TO_COMPLETE - 1);
    size_t resIndexes[NUM_NEIGHBORS_TO_FIND];
    double resDists[NUM_NEIGHBORS_TO_FIND];
    KNNResultSet<double> resultSet(NUM_NEIGHBORS_TO_FIND);

    // Determine number of deliveries needed to be completed
    unsigned numCompleted = 0, numPickedup = 0;
    Truck truck(truck_capacity);
    
    unsigned currentInt = initialPath.end_intersection;
    unsigned pickUpOrderNum = initialOrderNum;
    threadCurrent.push_back(initialPath);
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
        threadCurrent.push_back(nextPath);

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
    threadCurrent.push_back(toDepot);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Total time to find base: " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " ms\n";
    
    return threadCurrent;
}

std::vector<CourierSubpath> findRandomisedPath(const std::vector<DeliveryInfo>& deliveries,
                                               const std::vector<int>& depots,
                                               const float turn_penalty,
                                               const float truck_capacity,
                                               const PointVec& pointData,
                                               const CourierSubpath& initialPath,
                                               const unsigned initialOrderNum) {
    std::vector<CourierSubpath> threadCurrent;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    const unsigned NUM_TO_COMPLETE = deliveries.size();
    const size_t NUM_NEIGHBORS_TO_FIND = 3;
    
    typedef KDTreeSingleIndexDynamicAdaptor<L2_Simple_Adaptor<double, PointVec>,
                                            PointVec, 2> KDTreeType;
    KDTreeType threadIndex(2, pointData, KDTreeSingleIndexAdaptorParams(5));
    threadIndex.addPoints(0, NUM_TO_COMPLETE - 1);
    size_t resIndexes[NUM_NEIGHBORS_TO_FIND];
    double resDists[NUM_NEIGHBORS_TO_FIND];
    KNNResultSet<double> resultSet(NUM_NEIGHBORS_TO_FIND);

    // Determine number of deliveries needed to be completed
    unsigned numCompleted = 0, numPickedup = 0;
    Truck truck(truck_capacity);
    
    unsigned currentInt = initialPath.end_intersection;
    unsigned pickUpOrderNum = initialOrderNum;
    threadCurrent.push_back(initialPath);
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
        std::pair<unsigned, bool> option1, option2;
        double dist1 = std::numeric_limits<double>::max();
        double dist2 = std::numeric_limits<double>::max();
        int zeroDistOrder = -1;
        bool zeroDistPickup = false;

        if (numPickedup < NUM_TO_COMPLETE) {
            LatLon currentPos = getIntersectionPosition(currentInt);
            const double queryPoint[2] = {xFromLon(currentPos.lon()), yFromLat(currentPos.lat())};
            resultSet.init(resIndexes, resDists);
            threadIndex.findNeighbors(resultSet, queryPoint, nanoflann::SearchParams(5));
            unsigned minNeighbor = std::min((unsigned)NUM_NEIGHBORS_TO_FIND, (NUM_TO_COMPLETE - numPickedup));
            for (unsigned neighbor = 0; neighbor < minNeighbor; ++neighbor) {
                if (truck.curWeight + deliveries[resIndexes[neighbor]].itemWeight < truck.capacity) {
                    if (currentInt == deliveries[resIndexes[neighbor]].pickUp) {
                        zeroDistOrder = resIndexes[neighbor];
                        zeroDistPickup = true;
                        break;
                    }
                    double dist = find_distance_between_two_points(std::make_pair(
                                        getIntersectionPosition(currentInt),
                                        getIntersectionPosition(deliveries[resIndexes[neighbor]].pickUp)));
                    if (dist < dist1) {
                        option1.first = resIndexes[neighbor];
                        option1.second = true;
                    }
                    else if (dist < dist2) {
                        option2.first = resIndexes[neighbor];
                        option2.second = true;
                    }
                }
            }
        }
        if (zeroDistOrder == -1) {
            for (const unsigned& orderNum : truck.packages) {
                if (currentInt == deliveries[orderNum].dropOff) {
                    zeroDistOrder = orderNum;
                    zeroDistPickup = false;
                    break;
                }
                double dist = find_distance_between_two_points(std::make_pair(
                            getIntersectionPosition(currentInt),
                            getIntersectionPosition(deliveries[orderNum].dropOff)));
                if (dist < dist1) {
                    option1.first = orderNum;
                    option1.second = false;
                }
                else if (dist < dist2) {
                    option2.first = orderNum;
                    option2.second = false;
                }
            }
        }
        
        int nextOrder;
        bool isPickup;
        if (zeroDistOrder != -1) {
            nextOrder = zeroDistOrder;
            isPickup = zeroDistPickup;
        }
        else {
            unsigned randChoice = rand() % 10;
            if (randChoice < 7) {
                nextOrder = option1.first;
                isPickup = option1.second;
            }
            else {
                nextOrder = option2.first;
                isPickup = option2.second;
            }
        }
        
        
        //If the last path was to a pick-up, the current path is from the pick up, so pick up the package
        if (prevPathIsPickUp) {
            nextPath.pickUp_indices.push_back(pickUpOrderNum);
            prevPathIsPickUp = false;
        }

        //Construct the courier sub-path from current intersection to next location
        nextPath.start_intersection = currentInt;
        if (isPickup) {
            nextPath.end_intersection = deliveries[nextOrder].pickUp;
            prevPathIsPickUp = true;
            pickUpOrderNum = nextOrder;
            truck.addPackage(pickUpOrderNum, deliveries[pickUpOrderNum].itemWeight);
            threadIndex.removePoint(pickUpOrderNum);
            numPickedup++;
        }
        else {
            nextPath.end_intersection = deliveries[nextOrder].dropOff;
            truck.removePackage(nextOrder, deliveries[nextOrder].itemWeight);
            numCompleted++;
        }
        nextPath.subpath = find_path_between_intersections(nextPath.start_intersection, nextPath.end_intersection, turn_penalty);
        threadCurrent.push_back(nextPath);

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
    threadCurrent.push_back(toDepot);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Total time to find rand path: " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " ms\n";
    
    return threadCurrent;
}


//KDTreeType threadIndex(2, pointData, KDTreeSingleIndexAdaptorParams(5));
//            threadIndex.addPoints(0, NUM_TO_COMPLETE - 1);
//            size_t resIndexes[NUM_NEIGHBORS_TO_FIND];
//            double resDists[NUM_NEIGHBORS_TO_FIND];
//            KNNResultSet<double> resultSet(NUM_NEIGHBORS_TO_FIND);
//
//            // Determine number of deliveries needed to be completed
//            unsigned numCompleted = 0, numPickedup = 0;
//            Truck truck(truck_capacity);
//
//            //Construct the first sub path
//            CourierSubpath initialPath;
//            unsigned currentInt = mapItr->second.first;
//            unsigned pickUpOrderNum = mapItr->second.second;
//            initialPath.start_intersection = currentInt;
//            initialPath.end_intersection = deliveries[pickUpOrderNum].pickUp;
//            initialPath.subpath = find_path_between_intersections(initialPath.start_intersection, initialPath.end_intersection, turn_penalty);
//            threadBest.push_back(initialPath);
//            currentInt = initialPath.end_intersection;
//            bool isFirstPath = true, prevPathIsPickUp = true;
//
//            while (numCompleted < NUM_TO_COMPLETE) {
//                CourierSubpath toPickup, toDropoff, nextPath;
//                if (isFirstPath) {
//                    nextPath.pickUp_indices.push_back(pickUpOrderNum);
//                    truck.addPackage(pickUpOrderNum, deliveries[pickUpOrderNum].itemWeight);
//                    threadIndex.removePoint(pickUpOrderNum);
//                    numPickedup++;
//                    isFirstPath = false;
//                    prevPathIsPickUp = false;
//                }
//
//                bool isPickup = true;
//                int closestOrder = -1;
//                double closestDistance = std::numeric_limits<double>::max();
//
//                if (numPickedup < NUM_TO_COMPLETE) {
//                    LatLon currentPos = getIntersectionPosition(currentInt);
//                    const double queryPoint[2] = {xFromLon(currentPos.lon()), yFromLat(currentPos.lat())};
//                    resultSet.init(resIndexes, resDists);
//                    threadIndex.findNeighbors(resultSet, queryPoint, nanoflann::SearchParams(5));
//                    unsigned minNeighbor = std::min((unsigned)NUM_NEIGHBORS_TO_FIND, (NUM_TO_COMPLETE - numPickedup));
//                    for (unsigned neighbor = 0; neighbor < minNeighbor; ++neighbor) {
//                        if (truck.curWeight + deliveries[resIndexes[neighbor]].itemWeight < truck.capacity) {
//                            closestOrder = resIndexes[neighbor];
//                            closestDistance = find_distance_between_two_points(std::make_pair(
//                                                   getIntersectionPosition(currentInt),
//                                                   getIntersectionPosition(deliveries[closestOrder].pickUp)));
//                            break;
//                        }
//                    }
//                }
//                for (const unsigned& orderNum : truck.packages) {
//                    double distToOrder = find_distance_between_two_points(std::make_pair(
//                                            getIntersectionPosition(currentInt),
//                                            getIntersectionPosition(deliveries[orderNum].dropOff)));
//                    if (distToOrder < closestDistance) {
//                        closestDistance = distToOrder;
//                        closestOrder = orderNum;
//                        isPickup = false;
//                    }
//                }
//
//    //            auto endFindKD = std::chrono::high_resolution_clock::now();
//    //            timeKDTree += std::chrono::duration_cast<std::chrono::nanoseconds>(endFindKD - startFindKD);
//    //            std::cout << "Time taken to find nearest pickup with KD tree: " 
//    //                      << std::chrono::duration_cast<std::chrono::nanoseconds>(endFindKD - startFindKD).count() << " ns\n";
//    //            std::cout << "Closest index with KD tree: " << closestOrder << " Distance: " << closestDistance
//    //                      << " Pickup: " << (isPickup ? "Yes" : "No")
//    //                      << " Current weight: " << truck.curWeight << " Package size: " << deliveries[closestOrder].itemWeight << "\n";
//
//                //If the last path was to a pick-up, the current path is from the pick up, so pick up the package
//                if (prevPathIsPickUp) {
//                    nextPath.pickUp_indices.push_back(pickUpOrderNum);
//                    prevPathIsPickUp = false;
//                }
//
//                //Construct the courier sub-path from current intersection to next location
//                nextPath.start_intersection = currentInt;
//                if (isPickup) {
//                    nextPath.end_intersection = deliveries[closestOrder].pickUp;
//                    prevPathIsPickUp = true;
//                    pickUpOrderNum = closestOrder;
//                    truck.addPackage(pickUpOrderNum, deliveries[pickUpOrderNum].itemWeight);
//                    threadIndex.removePoint(pickUpOrderNum);
//                    numPickedup++;
//                }
//                else {
//                    nextPath.end_intersection = deliveries[closestOrder].dropOff;
//                    truck.removePackage(closestOrder, deliveries[closestOrder].itemWeight);
//                    numCompleted++;
//                }
//                nextPath.subpath = find_path_between_intersections(nextPath.start_intersection, nextPath.end_intersection, turn_penalty);
//                threadBest.push_back(nextPath);
//
//                currentInt = nextPath.end_intersection;
//            }
//
//            //Last stop to Depot
//            CourierSubpath toDepot;
//            toDepot.start_intersection = currentInt;
//
//            //Determine closest depot to go to
//            double closestDepotDistance = std::numeric_limits<double>::max();
//            unsigned closestDepot = 0;
//            for (unsigned depotNum = 0; depotNum < depots.size(); ++depotNum) {
//                double distToDepot = find_distance_between_two_points(std::make_pair(
//                                             getIntersectionPosition(currentInt),
//                                             getIntersectionPosition(depots[depotNum])));
//                if (distToDepot < closestDepotDistance) {
//                    closestDepotDistance = distToDepot;
//                    closestDepot = depotNum;
//                }
//            }
//            toDepot.end_intersection = depots[closestDepot];
//            toDepot.subpath = find_path_between_intersections(currentInt,
//                                                              depots[closestDepot],
//                                                              turn_penalty);
//            threadBest.push_back(toDepot);
