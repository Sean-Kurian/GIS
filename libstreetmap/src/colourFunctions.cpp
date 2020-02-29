//==============================================================================
// File Description: 
//
//==============================================================================

#include "colourFunctions.h"

//
ezgl::color getFeatureColour(const FeatureType& type) {
    switch (type) {
        case Unknown:
            return ezgl::RED;
        case Park:
            return ezgl::color(0xCB, 0xE6, 0xA3);
        case Beach:
            return ezgl::color(0xFF, 0xEF, 0xC3);
        case Lake:
            return ezgl::color(0xA2, 0xCD, 0xFC);
        case River:
            return ezgl::color(0xA2, 0xCD, 0xFC);
        case Island:
            return ezgl::color(0xE0, 0xE0, 0xE0);
        case Building:
            return ezgl::color(0x84, 0x94, 0xA4);
        case Greenspace:
            return ezgl::color(0xCB, 0xE6, 0xA3);
        case Golfcourse:
            return ezgl::color(0xCB, 0xE6, 0xA3);
        case Stream:
            return ezgl::color(0xA2, 0xCD, 0xFC);
        default:
            std::cerr << "Error: No matching feature type\n";
    }
    return ezgl::RED;
}

ezgl::color getRoadColour(const roadType& type) {
    switch (type) {
        case highway:
            return ezgl::color(0xFF, 0xF1, 0xBA);
        case majorRoad:
            return ezgl::color(0xFF, 0xFF, 0xFF);
        case minorRoad:
            return ezgl::color(0xF2, 0xF2, 0xF2);
        case trail:
            return ezgl::BLACK;
        case path:
            return ezgl::BLACK;
        default:
            std::cerr << "Error: no matching street type\n";
    }
    return ezgl::RED;
}

ezgl::color getBuildingColour(const buildingType& type) {
    switch (type) {
 //ezgl::color(0x35, 0x6C, 0xA3);
        case school:
            return ezgl::color(0x40, 0x9F, 0xC4);
        case hospital:
            return ezgl::color(0xC4, 0x68, 0x96);
        case other:
            return ezgl::color(0x84, 0x94, 0xA4);
        default:
            std::cerr << "Error: no matching building type\n";
    }
    return ezgl::RED;
}
