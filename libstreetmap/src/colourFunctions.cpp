//==============================================================================
// File Description: Contains functions which are used to determine the 
// appropriate colour of an object based on object type and whether its 
// currently in night mode or not
//==============================================================================
#include "colourFunctions.h"

// Declaration of extern variable which controls whether night mode is enabled
namespace colourMode {
    colourModes mode;
    colourModes operator++(colourModes& cMode, int) {
        const int i = static_cast<int>(mode) + 1;
        cMode = static_cast<colourModes>((i) % colourModes::CM_TYPECOUNT);
        return cMode;
    }
}

// Returns feature colour based on feature type and whether night mode is active
ezgl::color getFeatureColour(const FeatureType& type) {
    switch (colourMode::mode) {
        case colourMode::dayMode:
            switch (type) {
                case Unknown:
                    return ERROR_COLOUR;
                case Park:
                    return DAY_GRASS_COLOUR;
                case Beach:
                    return DAY_BEACH_COLOUR;
                case Lake:
                    return DAY_WATER_COLOUR;
                case River:
                    return DAY_WATER_COLOUR;
                case Island:
                    return DAY_BACKGROUND_COLOUR;
                case Building:
                    return DAY_BUILDING_COLOUR;
                case Greenspace:
                    return DAY_GRASS_COLOUR;
                case Golfcourse:
                    return DAY_GRASS_COLOUR;
                case Stream:
                    return DAY_WATER_COLOUR;
                default:
                    std::cerr << "Error: No matching feature type\n";
            }
        case colourMode::nightMode:
            switch (type) {
                case Unknown:
                    return ERROR_COLOUR;
                case Park:
                    return NIGHT_GRASS_COLOUR;
                case Beach:
                    return NIGHT_BEACH_COLOUR;
                case Lake:
                    return NIGHT_WATER_COLOUR;
                case River:
                    return NIGHT_WATER_COLOUR;
                case Island:
                    return NIGHT_BACKGROUND_COLOUR;
                case Building:
                    return NIGHT_BUILDING_COLOUR;
                case Greenspace:
                    return NIGHT_GRASS_COLOUR;
                case Golfcourse:
                    return NIGHT_GRASS_COLOUR;
                case Stream:
                    return NIGHT_WATER_COLOUR;
                default:
                    std::cerr << "Error: No matching feature type\n";
            }
        case colourMode::redGreen:
            switch (type) {
                case Unknown:
                    return ERROR_COLOUR;
                case Park:
                    return DAY_GRASS_COLOUR;
                case Beach:
                    return DAY_BEACH_COLOUR;
                case Lake:
                    return DAY_WATER_COLOUR;
                case River:
                    return DAY_WATER_COLOUR;
                case Island:
                    return DAY_BACKGROUND_COLOUR;
                case Building:
                    return RG_BUILDING_COLOUR;
                case Greenspace:
                    return DAY_GRASS_COLOUR;
                case Golfcourse:
                    return DAY_GRASS_COLOUR;
                case Stream:
                    return DAY_WATER_COLOUR;
                default:
                    std::cerr << "Error: No matching feature type\n";
            }
        case colourMode::blueYellow:
            switch (type) {
                case Unknown:
                    return ERROR_COLOUR;
                case Park:
                    return BY_GRASS_COLOUR;
                case Beach:
                    return BY_BEACH_COLOUR;
                case Lake:
                    return BY_WATER_COLOUR;
                case River:
                    return BY_WATER_COLOUR;
                case Island:
                    return DAY_BACKGROUND_COLOUR;
                case Building:
                    return BY_BUILDING_COLOUR;
                case Greenspace:
                    return BY_GRASS_COLOUR;
                case Golfcourse:
                    return BY_GRASS_COLOUR;
                case Stream:
                    return BY_WATER_COLOUR;
                default:
                    std::cerr << "Error: No matching feature type\n";
            }
        case colourMode::totalColour:
            switch (type) {
                case Unknown:
                    return ERROR_COLOUR;
                case Park:
                    return DAY_GRASS_COLOUR;
                case Beach:
                    return DAY_BEACH_COLOUR;
                case Lake:
                    return DAY_WATER_COLOUR;
                case River:
                    return DAY_WATER_COLOUR;
                case Island:
                    return DAY_BACKGROUND_COLOUR;
                case Building:
                    return DAY_BUILDING_COLOUR;
                case Greenspace:
                    return DAY_GRASS_COLOUR;
                case Golfcourse:
                    return DAY_GRASS_COLOUR;
                case Stream:
                    return DAY_WATER_COLOUR;
                default:
                    std::cerr << "Error: No matching feature type\n";
            }
        default:
            switch (type) {
                case Unknown:
                    return ERROR_COLOUR;
                case Park:
                    return DAY_GRASS_COLOUR;
                case Beach:
                    return DAY_BEACH_COLOUR;
                case Lake:
                    return DAY_WATER_COLOUR;
                case River:
                    return DAY_WATER_COLOUR;
                case Island:
                    return DAY_BACKGROUND_COLOUR;
                case Building:
                    return DAY_BUILDING_COLOUR;
                case Greenspace:
                    return DAY_GRASS_COLOUR;
                case Golfcourse:
                    return DAY_GRASS_COLOUR;
                case Stream:
                    return DAY_WATER_COLOUR;
                default:
                    std::cerr << "Error: No matching feature type\n";
            }
    }
    return ERROR_COLOUR;
}

// Returns road colour based on road type and whether night mode is active
ezgl::color getRoadColour(const roadType& type) {
    switch (colourMode::mode) {
        case colourMode::dayMode:
            switch (type) {
                case highway:
                    return DAY_HIGHWAY_COLOUR;
                case majorRoad:
                    return DAY_MAJOR_ROAD_COLOUR;
                case minorRoad:
                    return DAY_MINOR_ROAD_COLOUR;
                case trail:
                    return DAY_TRAIL_COLOUR;
                case path:
                    return DAY_PATH_COLOUR;
                case highlightedDrive:
                    return DAY_HIGHLIGHT_DRIVE_COLOUR;
                case highlightedWalk:
                    return DAY_HIGHLIGHT_WALK_COLOUR;
                default:
                    std::cerr << "Error: no matching street type\n";
            }
        case colourMode::nightMode:
            switch (type) {
                case highway:
                    return NIGHT_HIGHWAY_COLOUR;
                case majorRoad:
                    return NIGHT_MAJOR_ROAD_COLOUR;
                case minorRoad:
                    return NIGHT_MINOR_ROAD_COLOUR;
                case trail:
                    return NIGHT_TRAIL_COLOUR;
                case path:
                    return NIGHT_PATH_COLOUR;
                case highlightedDrive:
                    return NIGHT_HIGHLIGHT_DRIVE_COLOUR;
                case highlightedWalk:
                    return NIGHT_HIGHLIGHT_WALK_COLOUR;
                default:
                    std::cerr << "Error: no matching street type\n";
            }
        case colourMode::redGreen:
            switch (type) {
                case highway:
                    return DAY_HIGHWAY_COLOUR;
                case majorRoad:
                    return DAY_MAJOR_ROAD_COLOUR;
                case minorRoad:
                    return DAY_MINOR_ROAD_COLOUR;
                case trail:
                    return DAY_TRAIL_COLOUR;
                case path:
                    return DAY_PATH_COLOUR;
                case highlightedDrive:
                    return DAY_HIGHLIGHT_DRIVE_COLOUR;
                case highlightedWalk:
                    return DAY_HIGHLIGHT_WALK_COLOUR;
                default:
                    std::cerr << "Error: no matching street type\n";
            }
        case colourMode::blueYellow:
            switch (type) {
                case highway:
                    return DAY_HIGHWAY_COLOUR;
                case majorRoad:
                    return DAY_MAJOR_ROAD_COLOUR;
                case minorRoad:
                    return DAY_MINOR_ROAD_COLOUR;
                case trail:
                    return DAY_TRAIL_COLOUR;
                case path:
                    return DAY_PATH_COLOUR;
                case highlightedDrive:
                    return DAY_HIGHLIGHT_DRIVE_COLOUR;
                case highlightedWalk:
                    return DAY_HIGHLIGHT_WALK_COLOUR;
                default:
                    std::cerr << "Error: no matching street type\n";
            }
        case colourMode::totalColour:
            switch (type) {
                case highway:
                    return DAY_HIGHWAY_COLOUR;
                case majorRoad:
                    return DAY_MAJOR_ROAD_COLOUR;
                case minorRoad:
                    return DAY_MINOR_ROAD_COLOUR;
                case trail:
                    return DAY_TRAIL_COLOUR;
                case path:
                    return DAY_PATH_COLOUR;
                case highlightedDrive:
                    return DAY_HIGHLIGHT_DRIVE_COLOUR;
                case highlightedWalk:
                    return DAY_HIGHLIGHT_WALK_COLOUR;
                default:
                    std::cerr << "Error: no matching street type\n";
            }
        default:
            switch (type) {
                case highway:
                    return DAY_HIGHWAY_COLOUR;
                case majorRoad:
                    return DAY_MAJOR_ROAD_COLOUR;
                case minorRoad:
                    return DAY_MINOR_ROAD_COLOUR;
                case trail:
                    return DAY_TRAIL_COLOUR;
                case path:
                    return DAY_PATH_COLOUR;
                case highlightedDrive:
                    return DAY_HIGHLIGHT_DRIVE_COLOUR;
                case highlightedWalk:
                    return DAY_HIGHLIGHT_WALK_COLOUR;
                default:
                    std::cerr << "Error: no matching street type\n";
            }
    }
    return ERROR_COLOUR;
}

// Returns building colour based on building type and whether night mode is active
ezgl::color getBuildingColour(const buildingType& type) {
    switch (colourMode::mode) {
        case colourMode::dayMode:
            switch (type) {
                case school:
                    return DAY_SCHOOL_COLOUR;
                case hospital:
                    return DAY_HOSPITAL_COLOUR;
                case other:
                    return DAY_BUILDING_COLOUR;
                default:
                    std::cerr << "Error: no matching building type\n";
            }
        case colourMode::nightMode:
            switch (type) {
                case school:
                    return NIGHT_SCHOOL_COLOUR;
                case hospital:
                    return NIGHT_HOSPITAL_COLOUR;
                case other:
                    return NIGHT_BUILDING_COLOUR;
                default:
                    std::cerr << "Error: no matching building type\n";
            }
        case colourMode::redGreen:
            switch (type) {
                case school:
                    return RG_SCHOOL_COLOUR;
                case hospital:
                    return RG_HOSPITAL_COLOUR;
                case other:
                    return RG_BUILDING_COLOUR;
                default:
                    std::cerr << "Error: no matching building type\n";
            }
        case colourMode::blueYellow:
            switch (type) {
                case school:
                    return BY_SCHOOL_COLOUR;
                case hospital:
                    return BY_HOSPITAL_COLOUR;
                case other:
                    return BY_BUILDING_COLOUR;
                default:
                    std::cerr << "Error: no matching building type\n";
            }
        case colourMode::totalColour:
            switch (type) {
                case school:
                    return TC_SCHOOL_COLOUR;
                case hospital:
                    return TC_HOSPITAL_COLOUR;
                case other:
                    return TC_BUILDING_COLOUR;
                default:
                    std::cerr << "Error: no matching building type\n";
            }
        default:
            switch (type) {
                case school:
                    return DAY_SCHOOL_COLOUR;
                case hospital:
                    return DAY_HOSPITAL_COLOUR;
                case other:
                    return DAY_BUILDING_COLOUR;
                default:
                    std::cerr << "Error: no matching building type\n";
            }
    }
    return ERROR_COLOUR;
}