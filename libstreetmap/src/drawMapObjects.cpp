//==============================================================================
// File Description: 
//
//==============================================================================

#include "drawMapObjects.h"
#include "drawMapHelpers.h"

void drawStreets(ezgl::renderer* rend, const roadType& type, const unsigned& roadWidth) {
    rend->set_color(getRoadColour(type));
    std::vector<std::pair<int, unsigned> > segs = gData.getSegsOfStreetType(type);
    ezgl::point2d fromPos(0, 0), toPos(0, 0);
    for (const auto SSIndex : segs) {
        rend->set_line_width(roadWidth * SSIndex.second);
        InfoStreetSegment SSData = getInfoStreetSegment(SSIndex.first);
        unsigned numCurves = SSData.curvePointCount;
        LatLon intPos = getIntersectionPosition(SSData.from);
        fromPos = ezgl::point2d(xFromLon(intPos.lon()), yFromLat(intPos.lat()));
        for (unsigned curveIndex = 0; curveIndex < numCurves; ++curveIndex) {
            LatLon curvePos = getStreetSegmentCurvePoint(curveIndex, SSIndex.first);
            toPos = ezgl::point2d(xFromLon(curvePos.lon()), yFromLat(curvePos.lat()));
            rend->draw_line(fromPos, toPos);
            fromPos = toPos;
        }
        intPos = getIntersectionPosition(SSData.to);
        toPos = ezgl::point2d(xFromLon(intPos.lon()), yFromLat(intPos.lat()));
        rend->draw_line(fromPos, toPos);
    }
    double lon = lonFromX(100); 
    double lat = latFromY(150); 
    std::cout << "Lon, lat is " << lon << "," << lat << "\n"; 
    std::cout << "x, y is " << xFromLon(lon) << "," << yFromLat(lat) << "\n"; 
}

//
void drawAllFeatures(ezgl::renderer* rend) {
    drawFeatures(rend, naturalFeature::park);
    drawFeatures(rend, naturalFeature::forest);
    drawFeatures(rend, naturalFeature::water);
    drawFeatures(rend, naturalFeature::island);
    drawFeatures(rend, naturalFeature::beach);
}

//
void drawFeatures(ezgl::renderer* rend, const naturalFeature& type) {
    rend->set_line_width(4);
    std::vector<unsigned> features = gData.getIndexesOfNaturalFeature(type);
    for (const unsigned featureIndex : features) {
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
void drawBuildings(ezgl::renderer* rend);

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
            return ezgl::color(0x8A, 0xC7, 0x63);
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
