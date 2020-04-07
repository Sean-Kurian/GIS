#include "m1.h"
#include "m4.h"
#include "pathfinding.h"
#include "m4Helpers.h"

#include <limits>
#include <cstdlib>
#include <unordered_set>

std::vector<CourierSubpath> traveling_courier(const std::vector<DeliveryInfo>& deliveries,
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
            truck.packages.insert(std::make_pair(pickUpOrderNum, pickUpOrderNum));
            isFirstPath = false;
            prevPathIsPickUp = false;
        }
        
        //Determine the closest pickup or drop off location from current intersection
        double closestDistance = std::numeric_limits<double>::max();
        unsigned closestOrder = 0;
        bool isPickUp = true;
        
        for (unsigned orderNum = 0; orderNum < NUM_TO_COMPLETE; ++orderNum) {
            double distToOrder = std::numeric_limits<double>::max();
            bool pickUp;
            if (!orderComplete[orderNum]) {
                //Determine if order is not on truck, then find it's pickup distance
                if (!truck.packages.count(orderNum)) {
                    //First ensure there is room on the truck to possibly pick up the package
                    float currentWeight = getCurrentWeight(deliveries, truck.packages);
                    
                    if (currentWeight + deliveries[orderNum].itemWeight < truck.capacity) {
                        distToOrder = find_distance_between_two_points(std::make_pair(
                                     getIntersectionPosition(currentInt),
                                     getIntersectionPosition(deliveries[orderNum].pickUp)));
                        pickUp = true;
                    }
                }
                
                //Otherwise order is on truck, so find it's drop off distance
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
        
        //If the last path was to a pick-up, the current path is from the pick up, so pick up the package
        if (prevPathIsPickUp) {
            nextPath.pickUp_indices.push_back(pickUpOrderNum);
            truck.packages.insert(std::make_pair(pickUpOrderNum, pickUpOrderNum));
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
            truck.packages.erase(closestOrder);
            orderComplete[closestOrder] = true;
            numCompleted++;
        }
        

        nextPath.subpath = find_path_between_intersections(nextPath.start_intersection, nextPath.end_intersection, turn_penalty);
        result.push_back(nextPath);
        
//        toPickup.start_intersection = currentInt;
//        toPickup.end_intersection = deliveries[closestOrder].pickUp;
//        toPickup.subpath = find_path_between_intersections(currentInt, 
//                                                           deliveries[closestOrder].pickUp, 
//                                                           turn_penalty);
//        result.push_back(toPickup);
//        
//        toDropoff.start_intersection = deliveries[closestOrder].pickUp;
//        toDropoff.end_intersection = deliveries[closestOrder].dropOff;
//        toDropoff.subpath = find_path_between_intersections(deliveries[closestOrder].pickUp,
//                                                            deliveries[closestOrder].dropOff,
//                                                            turn_penalty);
//        toDropoff.pickUp_indices.push_back(closestOrder);
//        result.push_back(toDropoff);
        
//        orderComplete[closestOrder] = true;
        currentInt = nextPath.end_intersection;
//        numCompleted++;
    }
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
