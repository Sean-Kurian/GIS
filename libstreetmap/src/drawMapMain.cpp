//==============================================================================
// File Description: 
//
//==============================================================================

#include "m1.h"
#include "m2.h"
#include "StreetsDatabaseAPI.h"
#include "globalData.h"
#include "drawMapHelpers.h"

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

#include <iostream>

void drawMainCanvas(ezgl::renderer* rend);
void drawFeatures(ezgl::renderer* rend);
ezgl::color getColour(FeatureType type);


//
void draw_map() {
    ezgl::application::settings settings;
    settings.main_ui_resource = "libstreetmap/resources/main.ui";
    settings.window_identifier = "MainWindow"; 
    settings.canvas_identifier = "MainCanvas";  
    ezgl::application application(settings); 
    
    std::cout << "MinLat / MaxLon: " << gData.getMinLat() << " " << gData.getMaxLon() << "\n";
    ezgl::point2d maxPt(xFromLon(gData.getMaxLon()), yFromLat(gData.getMinLat()));
    std::cout << "Max point: " << maxPt.x << " " << maxPt.y << "\n";
    ezgl::rectangle mapCoords{ {0, 0}, maxPt };
    
    application.add_canvas("MainCanvas", drawMainCanvas, mapCoords);
    
    application.run(nullptr, nullptr, nullptr, nullptr);
}

//
void drawMainCanvas(ezgl::renderer* rend) {
    rend->set_line_width(4);
    for (unsigned featureIndex = 0; featureIndex < getNumFeatures(); ++featureIndex) {
        rend->set_color(getColour(getFeatureType((featureIndex))));
        LatLon pointLL;
        std::vector<ezgl::point2d> points;
        unsigned numPoints = getFeaturePointCount(featureIndex);
        for (unsigned point = 0; point < numPoints; ++point) {
            pointLL = getFeaturePoint(point, featureIndex);
            points.push_back(ezgl::point2d(xFromLon(pointLL.lon()), yFromLat(pointLL.lat())));
        }
        
        if (points.size() > 1) {
            if (points.front() == points.back())
                rend->fill_poly(points);
            else
                for (unsigned i = 0; i < points.size() - 1; ++i)
                    rend->draw_line(points[i], points[i + 1]);
        }
    }
}

//
ezgl::color getColour(FeatureType type) {
    switch (type) {
        case Unknown:
            return ezgl::RED;
        case Park:
            return ezgl::LIME_GREEN;
        case Beach:
            return ezgl::YELLOW;
        case Lake:
            return ezgl::LIGHT_SKY_BLUE;
        case River:
            return ezgl::LIGHT_SKY_BLUE;
        case Island:
            return ezgl::LIME_GREEN;
        case Building:
            return ezgl::BLACK;
        case Greenspace:
            return ezgl::LIME_GREEN;
        case Golfcourse:
            return ezgl::LIME_GREEN;
        case Stream:
            return ezgl::LIGHT_SKY_BLUE;
        default:
            std::cerr << "Error: No matching type. Fix yo shit\n";
    }
    return ezgl::RED;
}