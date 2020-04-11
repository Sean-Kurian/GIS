#ifndef COLOURFUNCTIONS_H
#define COLOURFUNCTIONS_H

//==============================================================================
// File Description: Declares all colours our map will use and contains functions
// which are used to determine the appropriate colour of an object based on
// object type and whether its currently in night mode or not
//==============================================================================

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

#include "roadTypes.h"
#include "buildingTypes.h"

// Feature Colours
#define DAY_WATER_COLOUR ezgl::color(0xA2, 0xCD, 0xFC)
#define DAY_GRASS_COLOUR ezgl::color(0xCB, 0xE6, 0xA3)
#define DAY_BEACH_COLOUR ezgl::color(0xFF, 0xEF, 0xC3)
#define DAY_BACKGROUND_COLOUR ezgl::color(0xE0, 0xE0, 0xE0)

#define NIGHT_WATER_COLOUR ezgl::color(0x00, 0x2B, 0x44)
#define NIGHT_GRASS_COLOUR ezgl::color(0x30, 0x67, 0x44)
#define NIGHT_BEACH_COLOUR ezgl::color(0xF7, 0xF2, 0xC7)
#define NIGHT_BACKGROUND_COLOUR ezgl::color(0x24, 0x2D, 0x37)

#define BY_WATER_COLOUR ezgl::color(0x3F, 0x72, 0xFC)
#define BY_GRASS_COLOUR ezgl::color(0x42, 0xFC, 0x3F)
#define BY_BEACH_COLOUR ezgl::color(0xD9, 0xFC, 0xD2)

// Road Colours
#define DAY_HIGHWAY_COLOUR ezgl::color(0xFF, 0xF1, 0xBA)
#define DAY_MAJOR_ROAD_COLOUR ezgl::color(0xFF, 0xFF, 0xFF)
#define DAY_MINOR_ROAD_COLOUR ezgl::color(0xF2, 0xF2, 0xF2)
#define DAY_TRAIL_COLOUR ezgl::color(0x75, 0x2F, 0x11)
#define DAY_PATH_COLOUR ezgl::color(0x77, 0x19, 0x19)
#define DAY_HIGHLIGHT_DRIVE_COLOUR ezgl::color(0xED, 0x3F, 0x32)
#define DAY_HIGHLIGHT_WALK_COLOUR ezgl::color(0x32, 0xE0, 0xED)

#define NIGHT_HIGHWAY_COLOUR ezgl::color(0xED, 0x93, 0x02)
#define NIGHT_MAJOR_ROAD_COLOUR ezgl::color(0x4F, 0x57, 0x64)
#define NIGHT_MINOR_ROAD_COLOUR ezgl::color(0x37, 0x3F, 0x4D)
#define NIGHT_TRAIL_COLOUR ezgl::color(0x37, 0x3F, 0x4D)
#define NIGHT_PATH_COLOUR ezgl::color(0xF2, 0xF2, 0xF2)
#define NIGHT_HIGHLIGHT_DRIVE_COLOUR ezgl::color(0xA7, 0x2F, 0xE9)
#define NIGHT_HIGHLIGHT_WALK_COLOUR ezgl::color(0x32, 0xE0, 0xED)

// Building Colours
#define DAY_SCHOOL_COLOUR ezgl::color(0x57, 0x90, 0xC5)
#define DAY_HOSPITAL_COLOUR ezgl::color(0xC4, 0x68, 0x96)
#define DAY_BUILDING_COLOUR ezgl::color(0x74, 0x84, 0x94)

#define NIGHT_SCHOOL_COLOUR ezgl::color(0x4B, 0x7C, 0xAA)
#define NIGHT_HOSPITAL_COLOUR ezgl::color(0xC4, 0x68, 0x96)
#define NIGHT_BUILDING_COLOUR ezgl::color(0x57, 0x64, 0x77)

#define RG_SCHOOL_COLOUR ezgl::color(0x3F, 0x42, 0xFC)
#define RG_HOSPITAL_COLOUR ezgl::color(0xF5, 0x42, 0x9B)
#define RG_BUILDING_COLOUR ezgl::color(0x74, 0x84, 0x94)

#define BY_SCHOOL_COLOUR ezgl::color(0x57, 0x90, 0xC5)
#define BY_HOSPITAL_COLOUR ezgl::color(0xC4, 0x68, 0x96)
#define BY_BUILDING_COLOUR ezgl::color(0x74, 0x84, 0x94)

#define TC_SCHOOL_COLOUR ezgl::color(0x57, 0x90, 0xC5)
#define TC_HOSPITAL_COLOUR ezgl::color(0xC4, 0x68, 0x96)
#define TC_BUILDING_COLOUR ezgl::color(0x74, 0x84, 0x94)

#define ERROR_COLOUR ezgl::color(0xFF, 0x00, 0x00)

// Namespace with extern variable which controls whether night mode is enabled
namespace colourMode {
    enum colourModes {
        dayMode = 0,
        nightMode,
        redGreen,
        blueYellow,
        totalColour,
        CM_TYPECOUNT
    };
    colourModes operator++(colourModes& mode, int);
    extern colourModes mode;
}

// Returns feature colour based on feature type and whether night mode is active
ezgl::color getFeatureColour(const FeatureType& type);

// Returns road colour based on road type and whether night mode is active
ezgl::color getRoadColour(const roadType& type);

// Returns building colour based on building type and whether night mode is active
ezgl::color getBuildingColour(const buildingType& type);

#endif /* COLOURFUNCTIONS_H */

