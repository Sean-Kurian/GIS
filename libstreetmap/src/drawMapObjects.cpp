//==============================================================================
// File Description: 
//
//==============================================================================

#include "drawMapObjects.h"
#include "drawMapHelpers.h"

void drawStreets(ezgl::renderer* rend, const roadType& type) {
    rend->set_color(getRoadColour(type));
    std::vector<int> segs = gData.getSegsOfStreetType(type);
    ezgl::point2d fromPos(0, 0), toPos(0, 0);
    for (const int SSIndex : segs) {
        InfoStreetSegment SSData = getInfoStreetSegment(SSIndex);
        unsigned numCurves = SSData.curvePointCount;
        LatLon intPos = getIntersectionPosition(SSData.from);
        fromPos = ezgl::point2d(xFromLon(intPos.lon()), yFromLat(intPos.lat()));
        for (unsigned curveIndex = 0; curveIndex < numCurves; ++curveIndex) {
            LatLon curvePos = getStreetSegmentCurvePoint(curveIndex, SSIndex);
            toPos = ezgl::point2d(xFromLon(curvePos.lon()), yFromLat(curvePos.lat()));
            rend->draw_line(fromPos, toPos);
            fromPos = toPos;
        }
        intPos = getIntersectionPosition(SSData.to);
        toPos = ezgl::point2d(xFromLon(intPos.lon()), yFromLat(intPos.lat()));
        rend->draw_line(fromPos, toPos);
    }
}

void drawFeatures(ezgl::renderer* rend) {
    rend->set_line_width(4);
    for (unsigned featureIndex = 0; featureIndex < getNumFeatures(); ++featureIndex) {
        rend->set_color(getFeatureColour(getFeatureType((featureIndex))));
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
ezgl::color getFeatureColour(const FeatureType& type) {
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
            std::cerr << "Error: No matching feature type\n";
    }
    return ezgl::RED;
}

ezgl::color getRoadColour(const roadType& type) {
    switch (type) {
        case highway:
            return ezgl::YELLOW;
        case majorRoad:
            return ezgl::GREY_75;
        case minorRoad:
            return ezgl::GREY_55;
        case trail:
            return ezgl::BLACK;
        case path:
            return ezgl::BLACK;
        default:
            std::cerr << "Error: no matching street type\n";
    }
    return ezgl::RED;
}