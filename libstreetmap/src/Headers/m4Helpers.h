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

//
std::vector<int> nearestDelivOrDrops(const std::vector<DeliveryInfo>& deliveries, std::vector<bool>& orderComplete, unsigned startDepot); 

//
double percentFull(double maxWeight, double currentWeight); 

#endif /* M4HELPERS_H */

