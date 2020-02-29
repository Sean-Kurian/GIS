//==============================================================================
// File Description: 
//
//==============================================================================

#include "drawMapObjects.h"
#include "drawMapHelpers.h"
#include "colourFunctions.h"
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
    } 
}

//
void drawAllFeatures(ezgl::renderer* rend, const double& pixelsPerMeter) {
    drawFeatures(rend);
    drawStreams(rend, pixelsPerMeter);
}

void drawFeatures(ezgl::renderer* rend) {
    std::multimap<double, unsigned> areaOfFeatures = gData.getAreaOfFeatures();
    
    for (auto itr = areaOfFeatures.rbegin(); itr != areaOfFeatures.rend(); ++itr) {
        rend->set_color(getFeatureColour(getFeatureType((itr->second))));
        LatLon pointLL;
        std::vector<ezgl::point2d> points;
        unsigned numPoints = getFeaturePointCount(itr->second);
        for (unsigned point = 0; point < numPoints; ++point) {
            pointLL = getFeaturePoint(point, itr->second);
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
void drawStreams(ezgl::renderer* rend, const double& pixelsPerMeter) {
    // OSM States that for water to be a stream its width should be jumpable by
    // an average person. After testing it was found I can jump around 2.5 meters
    rend->set_line_width(std::floor(pixelsPerMeter * 2.5));
    rend->set_color(getFeatureColour(FeatureType::Stream));
    
    std::vector<unsigned> streams = gData.getIndexesOfStreams();
    for (const unsigned streamIndex : streams) {    
        LatLon pointLL;
        std::vector<ezgl::point2d> points;
        unsigned numPoints = getFeaturePointCount(streamIndex);
        for (unsigned point = 0; point < numPoints; ++point) {
            pointLL = getFeaturePoint(point, streamIndex);
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
void drawAllBuildings(ezgl::renderer* rend) {
    drawBuildings(rend, buildingType::other);
    drawBuildings(rend, buildingType::school);
    drawBuildings(rend, buildingType::hospital);
}

//
void drawBuildings(ezgl::renderer* rend, const buildingType& type) {
    rend->set_color(getBuildingColour(type));
    std::vector<unsigned> buildings = gData.getIndexesOfBuildingType(type);
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
    std::vector<unsigned> buildings = gData.getIndexesOfBuildingType(buildingType::other);
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

