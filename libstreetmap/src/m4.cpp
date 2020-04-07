#include "m1.h"
#include "m4.h"
#include "pathfinding.h"

#include <limits>
#include <cstdlib>
#include <unordered_set>

std::vector<CourierSubpath> traveling_courier(const std::vector<DeliveryInfo>& deliveries,
                                              const std::vector<int>& depots,
                                              const float turn_penalty,
                                              const float truck_capacity) {
    //Determine starting depot and number of deliveries needed to be completed
    unsigned randDepot = std::rand() % depots.size();
    unsigned currentInt = depots[randDepot];
    std::vector<bool> orderComplete;
    orderComplete.resize(deliveries.size(), false);
    const unsigned NUM_TO_COMPLETE = deliveries.size();
    unsigned numCompleted = 0;
    
    std::vector<CourierSubpath> result;

    while (numCompleted < NUM_TO_COMPLETE) {
        CourierSubpath toPickup, toDropoff;
        
        //Determine the closest pickup location from current intersection
        double closestDistance = std::numeric_limits<double>::max();
        unsigned closestOrder = 0;
        for (unsigned orderNum = 0; orderNum < NUM_TO_COMPLETE; ++orderNum) {
            if (!orderComplete[orderNum]) {
                double distToOrder = find_distance_between_two_points(std::make_pair(
                                     getIntersectionPosition(currentInt),
                                     getIntersectionPosition(deliveries[orderNum].pickUp)));
                if (distToOrder < closestDistance) {
                    closestDistance = distToOrder;
                    closestOrder = orderNum;
                }
            }
        }
        
        //Construct the courier sub-path from current intersection to delivery pickup
        toPickup.start_intersection = currentInt;
        toPickup.end_intersection = deliveries[closestOrder].pickUp;
        toPickup.subpath = find_path_between_intersections(currentInt, 
                                                           deliveries[closestOrder].pickUp, 
                                                           turn_penalty);
        result.push_back(toPickup);
        
        toDropoff.start_intersection = deliveries[closestOrder].pickUp;
        toDropoff.end_intersection = deliveries[closestOrder].dropOff;
        toDropoff.subpath = find_path_between_intersections(deliveries[closestOrder].pickUp,
                                                            deliveries[closestOrder].dropOff,
                                                            turn_penalty);
        toDropoff.pickUp_indices.push_back(closestOrder);
        result.push_back(toDropoff);
        
        orderComplete[closestOrder] = true;
        currentInt = deliveries[closestOrder].dropOff;
        numCompleted++;
    }
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
    result.push_back(toDepot);
    return result;
}

double currentWeight(const std::vector<DeliveryInfo>& deliveries, std::vector<int> deliveryIndices){
    double weight = 0; 
    for (int i = 0; i < deliveryIndices.size(); i++){
        weight += deliveries[deliveryIndices[i]]; 
    }
    return weight; 
}
