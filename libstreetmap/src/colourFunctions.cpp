//==============================================================================
// File Description: Contains functions which are used to determine the 
// appropriate colour of an object based on object type and whether its 
// currently in night mode or not
//==============================================================================
#include "colourFunctions.h"

// Declaration of extern variable which controls whether night mode is enabled
namespace nightMode {
    bool isOn = false;
}

// Returns feature colour based on feature type and whether night mode is active
ezgl::color getFeatureColour(const FeatureType& type) {
    switch (type) {
        case Unknown:
            return ERROR_COLOUR;
        case Park:
            return nightMode::isOn ? NIGHT_GRASS_COLOUR : DAY_GRASS_COLOUR;
        case Beach:
            return nightMode::isOn ? NIGHT_BEACH_COLOUR : DAY_BEACH_COLOUR;
        case Lake:
            return nightMode::isOn ? NIGHT_WATER_COLOUR : DAY_WATER_COLOUR;
        case River:
            return nightMode::isOn ? NIGHT_WATER_COLOUR : DAY_WATER_COLOUR;
        case Island:
            return nightMode::isOn ? NIGHT_BACKGROUND_COLOUR : DAY_BACKGROUND_COLOUR;
        case Building:
            return nightMode::isOn ? NIGHT_BUILDING_COLOUR : DAY_BUILDING_COLOUR;
        case Greenspace:
            return nightMode::isOn ? NIGHT_GRASS_COLOUR : DAY_GRASS_COLOUR;
        case Golfcourse:
            return nightMode::isOn ? NIGHT_GRASS_COLOUR : DAY_GRASS_COLOUR;
        case Stream:
            return nightMode::isOn ? NIGHT_WATER_COLOUR : DAY_WATER_COLOUR;
        default:
            std::cerr << "Error: No matching feature type\n";
    }
    return ERROR_COLOUR;
}

// Returns road colour based on road type and whether night mode is active
ezgl::color getRoadColour(const roadType& type) {
    switch (type) {
        case highway:
            return nightMode::isOn ? NIGHT_HIGHWAY_COLOUR : DAY_HIGHWAY_COLOUR;
        case majorRoad:
            return nightMode::isOn ? NIGHT_MAJOR_ROAD_COLOUR : DAY_MAJOR_ROAD_COLOUR;
        case minorRoad:
            return nightMode::isOn ? NIGHT_MINOR_ROAD_COLOUR : DAY_MINOR_ROAD_COLOUR;
        case trail:
            return nightMode::isOn ? NIGHT_TRAIL_COLOUR : DAY_TRAIL_COLOUR;
        case path:
            return nightMode::isOn ? NIGHT_PATH_COLOUR : DAY_PATH_COLOUR;
        case highlightedDrive:
            return nightMode::isOn ? NIGHT_HIGHLIGHT_DRIVE_COLOUR : DAY_HIGHLIGHT_DRIVE_COLOUR;
        case highlightedWalk:
            return nightMode::isOn ? NIGHT_HIGHLIGHT_WALK_COLOUR : DAY_HIGHLIGHT_WALK_COLOUR;
        default:
            std::cerr << "Error: no matching street type\n";
    }
    return ERROR_COLOUR;
}

// Returns building colour based on building type and whether night mode is active
ezgl::color getBuildingColour(const buildingType& type) {
    switch (type) {
        case school:
            return nightMode::isOn ? NIGHT_SCHOOL_COLOUR : DAY_SCHOOL_COLOUR;
        case hospital:
            return nightMode::isOn ? NIGHT_HOSPITAL_COLOUR : DAY_HOSPITAL_COLOUR;
        case other:
            return nightMode::isOn ? NIGHT_BUILDING_COLOUR : DAY_BUILDING_COLOUR;
        default:
            std::cerr << "Error: no matching building type\n";
    }
    return ERROR_COLOUR;
}