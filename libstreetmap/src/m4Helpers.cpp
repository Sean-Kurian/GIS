#include "m4Helpers.h"


double percentFull(double maxWeight, double currentWeight){
    return (currentWeight/maxWeight * 100); 
}
//Returns the current weight on the truck
float getCurrentWeight(const std::vector<DeliveryInfo>& deliveries, std::unordered_map<unsigned, unsigned> deliveryIndices){
    float weight = 0; 
    for (int i = 0; i < deliveryIndices.size(); i++){
        weight += deliveries[deliveryIndices[i]].itemWeight; 
    }
    return weight; 
}