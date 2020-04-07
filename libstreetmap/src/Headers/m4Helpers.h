#ifndef M4HELPERS_H
#define M4HELPERS_H
#include "m4.h"

float currentWeight(const std::vector<DeliveryInfo>& deliveries, std::vector<int> deliveryIndices); 

std::vector<int> nearestDelivOrDrops(const std::vector<DeliveryInfo>& deliveries, std::vector<bool>& orderComplete, unsigned startDepot); 

double percentFull(double maxWeight, double currentWeight); 

#include <unordered_map>
#include <vector>
#include "m4.h"

struct Truck {
    //Defines attributes about the truck and its current status
    
    Truck(float weight)
            : capacity(weight) {}
    
    float capacity;
    
    std::unordered_map<unsigned, unsigned> packages;
};

//Returns the current weight on the truck
float getCurrentWeight(const std::vector<DeliveryInfo>& deliveries, std::unordered_map<unsigned, unsigned> deliveryIndices);

#endif /* M4HELPERS_H */

