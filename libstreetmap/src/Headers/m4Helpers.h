#ifndef M4HELPERS_H
#define M4HELPERS_H

#include "StreetsDatabaseAPI.h"
#include "m4.h"

#include <unordered_map>
#include <vector>

struct Truck {
    //Defines attributes about the truck and its current status
    
    Truck(float weight, float total)
            : capacity(weight), curWeight(total) {}
    
    float capacity;
    float curWeight; 
    
    std::unordered_map<unsigned, unsigned> packages;
};

//
std::vector<int> nearestDelivOrDrops(const std::vector<DeliveryInfo>& deliveries, std::vector<bool>& orderComplete, unsigned startDepot); 

//
double percentFull(double maxWeight, double currentWeight); 

//Returns the current weight on the truck
float getCurrentWeight(const std::vector<DeliveryInfo>& deliveries, std::unordered_map<unsigned, unsigned> deliveryIndices);

void addWeight(Truck& delivVechicle, float newWeight); 

void removeWeight(Truck& delivVehicle, float newWeight); 

float getWeight(Truck& delivVehicle); 

void emptyTruck(Truck& delivVehicle); 

std::vector<int> nearestDelivOrDrops(const std::vector<DeliveryInfo>& deliveries, const std::vector<bool>& orderComplete, unsigned startDepot);

#endif /* M4HELPERS_H */

