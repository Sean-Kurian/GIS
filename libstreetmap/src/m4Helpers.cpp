#include "m4Helpers.h"
#include "m1.h"
#include "m4.h"
#include "pathfinding.h"

#include <vector> 
#include <limits>
#include <algorithm> 

double percentFull(double maxWeight, double currentWeight) {
    return (currentWeight/maxWeight * 100); 
}

float getCurrentWeight(const std::vector<DeliveryInfo>& deliveries, std::unordered_map<unsigned, unsigned> deliveryIndices){
    float weight = 0; 
    for (int i = 0; i < deliveryIndices.size(); i++){
        weight += deliveries[deliveryIndices[i]].itemWeight; 
    }
    return weight; 
}

std::vector<int> nearestDelivOrDrops(const std::vector<DeliveryInfo>& deliveries, const std::vector<bool>& orderComplete, unsigned startDepot){

    std::vector<int> closestInts;
    long unsigned constantNumNoReason = 5; 
    unsigned numDelivOrDrops = std::min(deliveries.size(), constantNumNoReason); 
   
    double closestDistance = std::numeric_limits<double>::max();
    unsigned closestOrder = 0;
        while(closestInts.size() < numDelivOrDrops && !(std::find(orderComplete.begin(), orderComplete.end(), false) != orderComplete.end())){
            for (unsigned orderNum = 0; orderNum < deliveries.size(); ++orderNum) {
                if (!orderComplete[orderNum]) {
                    double distToOrder = find_distance_between_two_points(std::make_pair(
                                         getIntersectionPosition(startDepot),
                                         getIntersectionPosition(deliveries[orderNum].pickUp)));
                    if (distToOrder < closestDistance && !(std::find(closestInts.begin(), closestInts.end(), orderNum) != closestInts.end())) {
                        closestDistance = distToOrder;
                        closestOrder = orderNum;
                    }
                }
            }
            closestInts.push_back(closestOrder); 
        }
        return closestInts;
        
        //Determine the closest pickup location from start intersection

        
}

void getWeight(Truck& delivVehicle){
    return delivVehicle.curWeight; 
}

void addWeight(Truck& delivVehicle, float newWeight){
    delivVehicle.curWeight += newWeight; 
}

void removeWeight(Truck& delivVehicle, float newWeight){
    delivVehicle.curWeight -= newWeight; 
}
void emptyTruck(Truck& delivVehicle){
    delivVehicle.curWeight = 0; 
}