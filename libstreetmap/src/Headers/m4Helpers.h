#ifndef M4HELPERS_H
#define M4HELPERS_H

#include "StreetsDatabaseAPI.h"
#include "m4.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

//Defines attributes about the truck and its current status
struct Truck {
    Truck(float _capacity) : capacity(_capacity), curWeight(0) {}
    
    float capacity;
    float curWeight; 
    
    std::unordered_set<unsigned> packages;
    
    void addPackage(const unsigned package, const float weight);
    void removePackage(const unsigned package, const float weight);
    void emptyTruck();
};

//Returns 5 nearest deliveries or drop off points
std::vector<int> nearestDelivOrDrops(const std::vector<DeliveryInfo>& deliveries, std::vector<bool>& orderComplete, unsigned startDepot); 

//Returns total weight as a percentage of max weight in a truck
double percentFull(double maxWeight, double currentWeight); 

//Returns the total time of a path
double findTotalPathTime(const std::vector<CourierSubpath>& subPaths, const double turn_penalty);

#endif /* M4HELPERS_H */

