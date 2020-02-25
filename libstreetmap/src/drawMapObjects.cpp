//==============================================================================
// File Description: 
//
//==============================================================================

#include "drawMapObjects.h"
#include "drawMapHelpers.h"
#include "m1.h"
#include <cmath>

constexpr double RAD_TO_DEG = 57.2957795131;

void drawStreets(ezgl::renderer* rend, const roadType& type, const double& pixelsPerMeter) {
    rend->set_color(getRoadColour(type));
    std::vector<std::pair<int, unsigned> > segs = gData.getSegsOfStreetType(type);
    ezgl::point2d fromPos(0, 0), toPos(0, 0);
    for (const std::pair<int, unsigned>& SSIndex : segs) {
        rend->set_line_width(std::floor(pixelsPerMeter * 5.0 * SSIndex.second));
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
}

//
void drawStreetNames(ezgl::renderer* rend, const roadType& type, const double& pixelsPerMeter) {
    rend->set_horiz_text_just(ezgl::text_just::center);
    rend->set_vert_text_just(ezgl::text_just::center);
    rend->set_color(ezgl::BLACK);
    
    std::vector<std::pair<int, unsigned> > segs = gData.getSegsOfStreetType(type);
    double width, height, angle, angleCheck, textHeight;
    ezgl::point2d fromPos(0, 0), toPos(0, 0), center(0, 0);
    LatLon fromPosLL, toPosLL;
    for (const std::pair<int, unsigned>& SSIndex : segs) {
        bool wasDrawn = false;
        InfoStreetSegment SSData = getInfoStreetSegment(SSIndex.first);
        std::string streetName = getStreetName(SSData.streetID);
        bool oneWay = SSData.oneWay;
        if (streetName != "<unknown>") { 
            fromPosLL = getIntersectionPosition(SSData.from);
            fromPos = ezgl::point2d(xFromLon(fromPosLL.lon()), yFromLat(fromPosLL.lat()));
            height = std::floor(pixelsPerMeter * 5.0 * SSIndex.second);
            
            textHeight = std::min(std::max(height * 0.75, 10.0), 24.0);
            
            unsigned numCurves = SSData.curvePointCount;
            if (numCurves == 0) {
                toPosLL = getIntersectionPosition(SSData.to);
                toPos = ezgl::point2d(xFromLon(toPosLL.lon()), yFromLat(toPosLL.lat()));
                width = find_distance_between_two_points(std::make_pair(fromPosLL, toPosLL));
                if (toPos.x > fromPos.x)
                    angle = atan2((toPos.y - fromPos.y), (toPos.x - fromPos.x)) * RAD_TO_DEG;
                else
                    angle = atan2((fromPos.y - toPos.y), (fromPos.x - toPos.x)) * RAD_TO_DEG;
                rend->set_text_rotation(angle);
                center = ezgl::point2d(((toPos.x + fromPos.x) * 0.5), (toPos.y + fromPos.y) * 0.5);
                
                do {
                    rend->set_font_size(textHeight);
                    wasDrawn = rend->draw_text(center, streetName, width, height);
                    textHeight -= 1.0;
                } while (!wasDrawn && textHeight >= 16.0);
            }
            else {
                for (unsigned curveIndex = 0; curveIndex < numCurves; ++curveIndex) {
                    toPosLL = getStreetSegmentCurvePoint(curveIndex, SSIndex.first);
                    toPos = ezgl::point2d(xFromLon(toPosLL.lon()), yFromLat(toPosLL.lat()));
                    width = find_distance_between_two_points(std::make_pair(fromPosLL, toPosLL));
                    if (toPos.x > fromPos.x)
                        angle = atan2((toPos.y - fromPos.y), (toPos.x - fromPos.x)) * RAD_TO_DEG;
                    else
                        angle = atan2((fromPos.y - toPos.y), (fromPos.x - toPos.x)) * RAD_TO_DEG;
                    rend->set_text_rotation(angle);
                    center = ezgl::point2d(((toPos.x + fromPos.x) * 0.5), (toPos.y + fromPos.y) * 0.5);
                    wasDrawn = rend->draw_text(center, streetName, width, height);
                    if (wasDrawn)
                        break;
                    fromPosLL = toPosLL;
                    fromPos = toPos;
                }
                if (!wasDrawn) {
                    toPosLL = getIntersectionPosition(SSData.to);
                    toPos = ezgl::point2d(xFromLon(toPosLL.lon()), yFromLat(toPosLL.lat()));
                    width = find_distance_between_two_points(std::make_pair(fromPosLL, toPosLL));
                    if (toPos.x > fromPos.x)
                        angle = atan2((toPos.y - fromPos.y), (toPos.x - fromPos.x)) * RAD_TO_DEG;
                    else
                        angle = atan2((fromPos.y - toPos.y), (fromPos.x - toPos.x)) * RAD_TO_DEG;
                    rend->set_text_rotation(angle);
                    center = ezgl::point2d(((toPos.x + fromPos.x) * 0.5), (toPos.y + fromPos.y) * 0.5);
                    wasDrawn = rend->draw_text(center, streetName, width, height);
                }
            }
        }
        /*if (oneWay){
            fromPosLL = getIntersectionPosition(SSData.from);
            fromPos = ezgl::point2d(xFromLon(fromPosLL.lon()), yFromLat(fromPosLL.lat()));
            height = std::floor(pixelsPerMeter * 5.0 * SSIndex.second);
            
            textHeight = std::min(std::max(height * 0.75, 10.0), 24.0);
            
            unsigned numCurves = SSData.curvePointCount;
            if (numCurves == 0) {
                toPosLL = getIntersectionPosition(SSData.to);
                toPos = ezgl::point2d(xFromLon(toPosLL.lon()), yFromLat(toPosLL.lat()));
                width = find_distance_between_two_points(std::make_pair(fromPosLL, toPosLL));
                if (toPos.x > fromPos.x)
                    angle = atan2((toPos.y - fromPos.y), (toPos.x - fromPos.x)) * RAD_TO_DEG;
                else
                    angle = atan2((fromPos.y - toPos.y), (fromPos.x - toPos.x)) * RAD_TO_DEG;
                rend->set_text_rotation(angle);
                center = ezgl::point2d(((toPos.x + fromPos.x) * 0.5), (toPos.y + fromPos.y) * 0.5);
                
                do {
                    rend->set_font_size(textHeight);
                    wasDrawn = rend->draw_text(center, "--->", width, height);
                    textHeight -= 1.0;
                } while (!wasDrawn && textHeight >= 14.0);
            }
            else {
                for (unsigned curveIndex = 0; curveIndex < numCurves; ++curveIndex) {
                    toPosLL = getStreetSegmentCurvePoint(curveIndex, SSIndex.first);
                    toPos = ezgl::point2d(xFromLon(toPosLL.lon()), yFromLat(toPosLL.lat()));
                    width = find_distance_between_two_points(std::make_pair(fromPosLL, toPosLL));
                    if (toPos.x > fromPos.x)
                        angle = atan2((toPos.y - fromPos.y), (toPos.x - fromPos.x)) * RAD_TO_DEG;
                    else
                        angle = atan2((fromPos.y - toPos.y), (fromPos.x - toPos.x)) * RAD_TO_DEG;
                    rend->set_text_rotation(angle);
                    center = ezgl::point2d(((toPos.x + fromPos.x) * 0.5), (toPos.y + fromPos.y) * 0.5);
                    wasDrawn = rend->draw_text(center, "------>", width, height);
                    if (wasDrawn)
                        break;
                    fromPosLL = toPosLL;
                    fromPos = toPos;
                }
                if (!wasDrawn) {
                    toPosLL = getIntersectionPosition(SSData.to);
                    toPos = ezgl::point2d(xFromLon(toPosLL.lon()), yFromLat(toPosLL.lat()));
                    width = find_distance_between_two_points(std::make_pair(fromPosLL, toPosLL));
                    if (toPos.x > fromPos.x)
                        angle = atan2((toPos.y - fromPos.y), (toPos.x - fromPos.x)) * RAD_TO_DEG;
                    else
                        angle = atan2((fromPos.y - toPos.y), (fromPos.x - toPos.x)) * RAD_TO_DEG;
                    rend->set_text_rotation(angle);
                    center = ezgl::point2d(((toPos.x + fromPos.x) * 0.5), (toPos.y + fromPos.y) * 0.5);
                    wasDrawn = rend->draw_text(center, "----->", width, height);
                }
            }
        }*/
    } 
}

//
void drawAllFeatures(ezgl::renderer* rend) {
    drawFeatures(rend, naturalFeature::lake);
    drawFeatures(rend, naturalFeature::island);
    drawFeatures(rend, naturalFeature::river);
    drawFeatures(rend, naturalFeature::forest);
    drawFeatures(rend, naturalFeature::park);
    drawFeatures(rend, naturalFeature::beach);
    drawFeatures(rend, naturalFeature::minorWater);
}

//
void drawFeatures(ezgl::renderer* rend, const naturalFeature& type) {
    rend->set_line_width(2);
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
void drawBuildings(ezgl::renderer* rend) {
    rend->set_color(getFeatureColour(FeatureType::Building));
    std::vector<unsigned> buildings = gData.getIndexesOfBuildings();
    for (const unsigned buildingIndex : buildings) {
        LatLon pointLL;
        std::vector<ezgl::point2d> points;
        unsigned numPoints = getFeaturePointCount(buildingIndex);
        for (unsigned point = 0; point < numPoints; ++point) {
            pointLL = getFeaturePoint(point, buildingIndex);
            points.push_back(ezgl::point2d(xFromLon(pointLL.lon()), yFromLat(pointLL.lat())));
        }
        
        if (points.size() > 1) {
            if (points.front() == points.back())
                rend->fill_poly(points);
            else
                for (unsigned i = 0; i < points.size() - 1; ++i){ 
                    rend->draw_line(points[i], points[i + 1]);
                }
        }
    }
}
 //Example, replace with relevant indices and icons
void drawPOI(ezgl::renderer* rend){
    std::vector<unsigned> buildings = gData.getIndexesOfBuildings();
    ezgl::surface* test = rend->load_png("/nfs/ug/homes-4/k/kurianse/ece297/work/mapper/libstreetmap/resources/small_image.png");
    for (const unsigned buildingIndex : buildings) {
        LatLon pointLL = getFeaturePoint(0, buildingIndex); 
        rend->draw_surface(test, ezgl::point2d(xFromLon(pointLL.lon()), yFromLat(pointLL.lat())));
    }
    rend->free_surface(test); 
}

//
void drawHighlightedData(ezgl::renderer* rend) {
    rend->set_color(ezgl::RED);
    HighlightedData data = gData.getHLData();
    if (!data.highlightedInts.empty()) {
        for (const unsigned intIndex : data.highlightedInts) {
            LatLon pos = getIntersectionPosition(intIndex);
            ezgl::point2d pointPos(xFromLon(pos.lon()), yFromLat(pos.lat()));
            rend->draw_elliptic_arc(pointPos, 5, 5, 0, 360);
        }
    }
}

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
            return ezgl::color(0xCB, 0xE6, 0xA3);
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