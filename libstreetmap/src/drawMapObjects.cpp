//==============================================================================
// File Description: 
//
//==============================================================================

#include "drawMapObjects.h"
#include "drawMapHelpers.h"
#include "colourFunctions.h"
#include "m1.h"
#include <cmath>
#include "libcurl.h"

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
void drawPaths(ezgl::renderer* rend, const double& pixelsPerMeter) {
    rend->set_line_dash(ezgl::line_dash::asymmetric_5_3);
    rend->set_color(getRoadColour(roadType::path));
    
    std::vector<std::pair<int, unsigned> > segs = gData.getSegsOfStreetType(roadType::path);
    ezgl::point2d fromPos(0, 0), toPos(0, 0);
    for (const std::pair<int, unsigned>& SSIndex : segs) {
        rend->set_line_width(std::floor(pixelsPerMeter * 2.0));
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
    rend->set_line_dash(ezgl::line_dash::none);
}

//
void drawStreetNames(ezgl::renderer* rend, const roadType& type, const double& pixelsPerMeter) {
    rend->set_horiz_text_just(ezgl::text_just::center);
    rend->set_vert_text_just(ezgl::text_just::center);
    rend->set_color(nightMode::isOn ? ezgl::WHITE : ezgl::BLACK);
    
    std::vector<std::pair<int, unsigned> > segs = gData.getSegsOfStreetType(type);
    double width, height, textHeight, angle;
    ezgl::point2d fromPos(0, 0), toPos(0, 0), curvePos(0, 0), nextCurvePos(0, 0), center(0, 0);
    LatLon fromPosLL, toPosLL, curvePosLL, nextCurvePosLL;
    for (const std::pair<int, unsigned>& SSIndex : segs) {
        bool wasDrawn = false;
        InfoStreetSegment SSData = getInfoStreetSegment(SSIndex.first);
        std::string streetName = getStreetName(SSData.streetID);
        bool oneWay = SSData.oneWay;
        if (streetName != "<unknown>") { 
            fromPosLL = getIntersectionPosition(SSData.from);
            fromPos = ezgl::point2d(xFromLon(fromPosLL.lon()), yFromLat(fromPosLL.lat()));
            toPosLL = getIntersectionPosition(SSData.to);
            toPos = ezgl::point2d(xFromLon(toPosLL.lon()), yFromLat(toPosLL.lat()));
                
            height = std::floor(pixelsPerMeter * 5.0 * SSIndex.second);
            textHeight = std::min(std::max(height * 0.75, 10.0), 24.0);
            
            unsigned numCurves = SSData.curvePointCount;
            if (numCurves == 0) {
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
                double maxWidth = 0, avgAngle = 0;
                ezgl::point2d maxPoint(0, 0);
                bool firstPair = true;
                std::map<double, std::pair<double, ezgl::point2d> > distAndAngleFromInt;
                
                for (unsigned curveIndex = 0; curveIndex < numCurves; ++curveIndex) {
                    curvePosLL = getStreetSegmentCurvePoint(curveIndex, SSIndex.first);
                    curvePos = ezgl::point2d(xFromLon(curvePosLL.lon()), yFromLat(curvePosLL.lat()));
                    
                    width = find_distance_between_two_points(std::make_pair(fromPosLL, curvePosLL));
                    if (curvePos.x > fromPos.x)
                        angle = atan2((curvePos.y - fromPos.y), (curvePos.x - fromPos.x)) * RAD_TO_DEG;
                    else
                        angle = atan2((fromPos.y - curvePos.y), (fromPos.x - curvePos.x)) * RAD_TO_DEG;
                    
                    distAndAngleFromInt.insert(std::make_pair(width, std::make_pair(angle, curvePos)));
                }
                width = find_distance_between_two_points(std::make_pair(fromPosLL, toPosLL));
                if (toPos.x > fromPos.x)
                    angle = atan2((toPos.y - fromPos.y), (toPos.x - fromPos.x)) * RAD_TO_DEG;
                else
                    angle = atan2((fromPos.y - toPos.y), (fromPos.x - toPos.x)) * RAD_TO_DEG;
                
                distAndAngleFromInt.insert(std::make_pair(width, std::make_pair(angle, toPos)));
                
                auto lastItr = distAndAngleFromInt.begin();
                for (auto itr = distAndAngleFromInt.begin(); itr != distAndAngleFromInt.end(); ++itr) {
                    if (firstPair) {
                        maxWidth = itr->first;
                        avgAngle = itr->second.first;
                        maxPoint = itr->second.second;
                        firstPair = false;
                        lastItr = itr;
                    }
                    else {
                        curvePos = lastItr->second.second;
                        toPos = itr->second.second;
                        if (toPos.x > curvePos.x)
                            angle = atan2((toPos.y - curvePos.y), (toPos.x - curvePos.x)) * RAD_TO_DEG;
                        else
                            angle = atan2((curvePos.y - toPos.y), (curvePos.x - toPos.x)) * RAD_TO_DEG;
                        
                        if (abs(avgAngle - angle) <= 5.0) {
                            maxWidth = itr->first;
                            avgAngle = (avgAngle + angle) / 2;
                            maxPoint = toPos;
                        }
                        else {
                            break;
                        }
                    }
                }
                rend->set_text_rotation(avgAngle);
                center = ezgl::point2d(((maxPoint.x + fromPos.x) * 0.5), (maxPoint.y + fromPos.y) * 0.5);
                
                do {
                    rend->set_font_size(textHeight);
                    wasDrawn = rend->draw_text(center, streetName, maxWidth, height);
                    textHeight -= 1.0;
                } while (!wasDrawn && textHeight >= 16.0);
            }
//                double maxWidth = 0, angleOfMaxWidth, widthCheck, angleCheck;
//                
//                curvePosLL = getStreetSegmentCurvePoint(0, SSIndex.first);
//                curvePos = ezgl::point2d(xFromLon(curvePosLL.lon()), yFromLat(curvePosLL.lat()));
//                
//                width = find_distance_between_two_points(std::make_pair(fromPosLL, curvePosLL));
//                if (curvePos.x > fromPos.x)
//                    angle = atan2((curvePos.y - fromPos.y), (curvePos.x - fromPos.x)) * RAD_TO_DEG;
//                else
//                    angle = atan2((fromPos.y - curvePos.y), (fromPos.x - curvePos.x)) * RAD_TO_DEG;
//                
//                
//                for (unsigned curveIndex = 1; curveIndex <= numCurves; ++curveIndex) {
//                    if (curveIndex != numCurves) {
//                        nextCurvePosLL = getStreetSegmentCurvePoint(curveIndex, SSIndex.first);
//                        nextCurvePos = ezgl::point2d(xFromLon(nextCurvePosLL.lon()), yFromLat(nextCurvePosLL.lat()));
//                    }
//                    else {
//                        nextCurvePosLL = toPosLL;
//                        nextCurvePos = toPos;
//                    }
//                    widthCheck = find_distance_between_two_points(std::make_pair(fromPosLL, nextCurvePosLL));
//                    
//                    if (width <= widthCheck) {
//                        if (nextCurvePos.x > curvePos.x)
//                            angleCheck = atan2((nextCurvePos.y - curvePos.y), (nextCurvePos.x - curvePos.x)) * RAD_TO_DEG;
//                        else
//                            angleCheck = atan2((curvePos.y - nextCurvePos.y), (curvePos.x - nextCurvePos.x)) * RAD_TO_DEG;
//                        
//                        if (abs(angle - angleCheck) <= 15) {
//                            if (maxWidth < width + find_distance_between_two_points(std::make_pair(curvePosLL, nextCurvePosLL))) {
//                                maxWidth = width + find_distance_between_two_points(std::make_pair(curvePosLL, nextCurvePosLL));
//                                angleOfMaxWidth = (angle + angleCheck) / 2;
//                            }
//                            else {
//                                
//                            }
//                               
//                            
//                            curvePosLL = nextCurvePosLL;
//                            curvePos = nextCurvePos;
//                            width = widthCheck;
//                            angle = angleCheck;
//                        }
//                        else
//                            break;
//                    }
//                    else {
//                        if (curvePos.x > nextCurvePos.x)
//                            angleCheck = atan2((curvePos.y - nextCurvePos.y), (curvePos.x - nextCurvePos.x)) * RAD_TO_DEG;
//                        else
//                            angleCheck = atan2((nextCurvePos.y - curvePos.y), (nextCurvePos.x - curvePos.x)) * RAD_TO_DEG;
//                        
//                        if (abs(angle - angleCheck) <= 15) {
//                            maxWidth = widthCheck;
//                            angleOfMaxWidth = (angle + angleCheck) / 2;
//                            
//                            curvePosLL = nextCurvePosLL;
//                            curvePos = nextCurvePos;
//                            width = widthCheck;
//                            angle = angleCheck;
//                        }
//                        else
//                            break;
//                    }
                
                
                
//                bool tooCurvy = false;
//                double distFromLine = 0;
//                toPosLL = getIntersectionPosition(SSData.to);
//                toPos = ezgl::point2d(xFromLon(toPosLL.lon()), yFromLat(toPosLL.lat()));
//                width = find_distance_between_two_points(std::make_pair(fromPosLL, toPosLL));
//                
//                for (unsigned curveIndex = 0; curveIndex < numCurves; ++curveIndex) {
//                    curvePosLL = getStreetSegmentCurvePoint(curveIndex, SSIndex.first);
//                    curvePos = ezgl::point2d(xFromLon(curvePosLL.lon()), yFromLat(curvePosLL.lat()));
//                    
//                    distFromLine = getDistFromLine(fromPos, toPos, curvePos);
//                    if (distFromLine >= halfHeight) {
//                        tooCurvy = true;
//                        std::cout << "Too thicc\n";
//                        break;
//                    }
//                }
//                if (!tooCurvy) {
//                    if (toPos.x > fromPos.x)
//                        angle = atan2((toPos.y - fromPos.y), (toPos.x - fromPos.x)) * RAD_TO_DEG;
//                    else
//                        angle = atan2((fromPos.y - toPos.y), (fromPos.x - toPos.x)) * RAD_TO_DEG;
//                    rend->set_text_rotation(angle);
//                    center = ezgl::point2d(((toPos.x + fromPos.x) * 0.5), (toPos.y + fromPos.y) * 0.5);
//
//                    do {
//                        rend->set_font_size(textHeight);
//                        wasDrawn = rend->draw_text(center, streetName, width, height);
//                        textHeight -= 1.0;
//                    } while (!wasDrawn && textHeight >= 16.0);
                
                
//                for (unsigned curveIndex = 0; curveIndex < numCurves; ++curveIndex) {
//                    toPosLL = getStreetSegmentCurvePoint(curveIndex, SSIndex.first);
//                    toPos = ezgl::point2d(xFromLon(toPosLL.lon()), yFromLat(toPosLL.lat()));
//                    width = find_distance_between_two_points(std::make_pair(fromPosLL, toPosLL));
//                    if (toPos.x > fromPos.x)
//                        angle = atan2((toPos.y - fromPos.y), (toPos.x - fromPos.x)) * RAD_TO_DEG;
//                    else
//                        angle = atan2((fromPos.y - toPos.y), (fromPos.x - toPos.x)) * RAD_TO_DEG;
//                    rend->set_text_rotation(angle);
//                    center = ezgl::point2d(((toPos.x + fromPos.x) * 0.5), (toPos.y + fromPos.y) * 0.5);
//                    wasDrawn = rend->draw_text(center, streetName, width, height);
//                    if (wasDrawn)
//                        break;
//                    fromPosLL = toPosLL;
//                    fromPos = toPos;
//                }
//                if (!wasDrawn) {
//                    toPosLL = getIntersectionPosition(SSData.to);
//                    toPos = ezgl::point2d(xFromLon(toPosLL.lon()), yFromLat(toPosLL.lat()));
//                    width = find_distance_between_two_points(std::make_pair(fromPosLL, toPosLL));
//                    if (toPos.x > fromPos.x)
//                        angle = atan2((toPos.y - fromPos.y), (toPos.x - fromPos.x)) * RAD_TO_DEG;
//                    else
//                        angle = atan2((fromPos.y - toPos.y), (fromPos.x - toPos.x)) * RAD_TO_DEG;
//                    rend->set_text_rotation(angle);
//                    center = ezgl::point2d(((toPos.x + fromPos.x) * 0.5), (toPos.y + fromPos.y) * 0.5);
//                    wasDrawn = rend->draw_text(center, streetName, width, height);
//                }
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

void PrintTTCVehicleInfo(ptree &ptRoot, ezgl::renderer* rend) {
    string busName;
    int busID = 0;
    double longitude = 0, latitude = 0;
    ezgl::surface* test = rend->load_png("/nfs/ug/homes-4/k/kurianse/ece297/work/mapper/libstreetmap/resources/plus.png");
    
    BOOST_FOREACH(ptree::value_type &featVal, ptRoot.get_child("features")) {
        // "features" maps to a JSON array, so each child should have no name
        
        if ( !featVal.first.empty() )
            throw "\"features\" child node has a name";

        busName = featVal.second.get<string>("properties.route_name");
        busID = featVal.second.get<int>("properties.vehicle_id");

        // Get GPS coordinates (stored as JSON array of 2 values)
        // Sanity checks: Only 2 values
        ptree coordinates = featVal.second.get_child("geometry.coordinates");
        if (coordinates.size() != 2)
            throw "Coordinates node does not contain 2 items";

        longitude = coordinates.front().second.get_value<double>();
        latitude = coordinates.back().second.get_value<double>();

        rend->draw_surface(test, ezgl::point2d(xFromLon(longitude), yFromLat(latitude)));
        rend->draw_surface(test, ezgl::point2d(-79.4325, 43.6525)); 
          
    }
    rend->free_surface(test); 
    
}
