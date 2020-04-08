#include "m4Helpers.h"
#include "m1.h"
#include "m4.h"
#include "pathfinding.h"

#include <vector> 
#include <limits>
#include <algorithm> 

void Truck::addPackage(const unsigned package, const float weight) {
    packages.insert(package);
    curWeight += weight;
}
void Truck::removePackage(const unsigned package, const float weight) {
    packages.erase(package);
    curWeight -= weight;
}
void Truck::emptyTruck() {
    curWeight = 0;
    packages.clear();
}

double percentFull(double maxWeight, double currentWeight) {
    return (currentWeight/maxWeight * 100); 
}

std::vector<int> nearestDelivOrDrops(const std::vector<DeliveryInfo>& deliveries, std::vector<bool>& orderComplete, unsigned startDepot) {

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

//Returns the total time of a path
double findTotalPathTime(const std::vector<CourierSubpath> subPaths, const double turn_penalty) {
    double totalTime = 0;
    for (int pathNum = 0; pathNum < subPaths.size(); ++pathNum) {
        totalTime += compute_path_travel_time(subPaths[pathNum].subpath, turn_penalty);
    }
    return totalTime;
}