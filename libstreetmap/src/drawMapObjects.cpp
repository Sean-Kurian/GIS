//==============================================================================
// File Description: Functions to draw individual objects on map such as streets, 
// buildings, icons etc. Also functions to draw highlighted data for user
//==============================================================================

#include "m1.h"
#include "drawMapObjects.h"
#include "drawMapHelpers.h"
#include "colourFunctions.h"
#include "libcurl.h"

#include <limits>
#include <cmath>

//Draws streets with their width varying based on the number of lanes
void drawStreets(ezgl::renderer* rend, const roadType& type, const double& pixelsPerMeter) {
    // Sets colour based on the type of street segments being drawn
    ezgl::color defaultRoadColour = getRoadColour(type);
    rend->set_color(defaultRoadColour);
    const HighlightedData& hlData = gData.getHLData();
    bool colourWasChanged = false;
    std::vector<std::pair<int, unsigned> > segs = gData.getSegsOfStreetType(type);
    ezgl::point2d fromPos(0, 0), toPos(0, 0);
    
    // Loops over all segments of a given street type and draws them
    for (const std::pair<int, unsigned>& SSIndex : segs) {
        // Checks if the colour was changed during the last loop and sets it back
        if (colourWasChanged) {
            rend->set_color(defaultRoadColour);
            colourWasChanged = false;
        }
        // Checks if the segments being drawn needs to be highlighted
        if (hlData.highlightedSegs[SSIndex.first] != highlightType::none) {
            if (hlData.highlightedSegs[SSIndex.first] == highlightType::driveHighlight)
                rend->set_color(getRoadColour(roadType::highlightedDrive));
            else
                rend->set_color(getRoadColour(roadType::highlightedWalk));
            // Used to ensure we don't have to set the colour for every seg
            colourWasChanged = true;
        }
        // Calculates the real life lane width. 5m is the average lane width worldwide
        rend->set_line_width(std::floor(pixelsPerMeter * 5.0 * SSIndex.second));
        InfoStreetSegment SSData = getInfoStreetSegment(SSIndex.first);
        unsigned numCurves = SSData.curvePointCount;
        LatLon intPos = getIntersectionPosition(SSData.from);
        fromPos = ezgl::point2d(xFromLon(intPos.lon()), yFromLat(intPos.lat()));
        // Draws the segment between each curve point
        for (unsigned curveIndex = 0; curveIndex < numCurves; ++curveIndex) {
            LatLon curvePos = getStreetSegmentCurvePoint(curveIndex, SSIndex.first);
            toPos = ezgl::point2d(xFromLon(curvePos.lon()), yFromLat(curvePos.lat()));
            rend->draw_line(fromPos, toPos);
            fromPos = toPos;
        }
        // Draws rest of the segment from the last curve point to the "to" intersection
        intPos = getIntersectionPosition(SSData.to);
        toPos = ezgl::point2d(xFromLon(intPos.lon()), yFromLat(intPos.lat()));
        rend->draw_line(fromPos, toPos);
    }
}

// Draws paths which are things such as trails. Intended for walking not cars
void drawPaths(ezgl::renderer* rend, const double& pixelsPerMeter) {
    // Makes the lines dotted to distinguish them
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
        // Draws the path between each curve point
        for (unsigned curveIndex = 0; curveIndex < numCurves; ++curveIndex) {
            LatLon curvePos = getStreetSegmentCurvePoint(curveIndex, SSIndex.first);
            toPos = ezgl::point2d(xFromLon(curvePos.lon()), yFromLat(curvePos.lat()));
            rend->draw_line(fromPos, toPos);
            fromPos = toPos;
        }
        // Draws the rest of the path from the last curve point to the "to" intersection
        intPos = getIntersectionPosition(SSData.to);
        toPos = ezgl::point2d(xFromLon(intPos.lon()), yFromLat(intPos.lat()));
        rend->draw_line(fromPos, toPos);
    }
    // Removes line dash to ensure roads are drawn correctly
    rend->set_line_dash(ezgl::line_dash::none);
}



// Draws the names of streets based on the visible size of a segment. Draws one way arrows
void drawStreetNames(ezgl::renderer* rend, const roadType& type, const double& pixelsPerMeter) {
    rend->set_horiz_text_just(ezgl::text_just::center);
    rend->set_vert_text_just(ezgl::text_just::center);
    // Sets name colour based on whether night mode is on to ensure its readable
    rend->set_color(nightMode::isOn ? ezgl::WHITE : ezgl::BLACK);
    
    std::vector<std::pair<int, unsigned> > segs = gData.getSegsOfStreetType(type);
    // Initial declarations of variables needed to draw names
    double width, height, textHeight, minTextHeight, angle;
    ezgl::point2d fromPos(0, 0), toPos(0, 0), curvePos(0, 0), nextCurvePos(0, 0), center(0, 0);
    LatLon fromPosLL, toPosLL, curvePosLL, nextCurvePosLL;
    // One way arrows which will indicate which way you can travel down a segment
    std::string rightArrow = "→", leftArrow = "←";
    
    for (const std::pair<int, unsigned>& SSIndex : segs) {
        bool wasDrawn = false; // Changes if the segment is drawn
        // Changes if the angle calculated was flipped to ensure name is drawn right-side up
        bool wasFlipped = false; 
        InfoStreetSegment SSData = getInfoStreetSegment(SSIndex.first);
        std::string streetName = getStreetName(SSData.streetID);
        bool oneWay = SSData.oneWay;
        
        if (streetName != "<unknown>") { // Unknown roads don't need street names drawn
            fromPosLL = getIntersectionPosition(SSData.from);
            fromPos = ezgl::point2d(xFromLon(fromPosLL.lon()), yFromLat(fromPosLL.lat()));
            toPosLL = getIntersectionPosition(SSData.to);
            toPos = ezgl::point2d(xFromLon(toPosLL.lon()), yFromLat(toPosLL.lat()));
            
            // Checks if the segment is visible to the user, if its not skip it
            if (!segOnScreen(rend, fromPos, toPos))
                continue;
            
            // Same height as that used when drawing the segments
            height = std::floor(pixelsPerMeter * 5.0 * SSIndex.second);
            // Minimum text height to ensure text is not drawn smaller than that
            minTextHeight = 8.0 + std::floor(pixelsPerMeter) * 3;
            // Finds text height and ensures it not too big or small
            textHeight = std::min(std::max(height * 0.75, minTextHeight), 24.0);
            
            unsigned numCurves = SSData.curvePointCount;
            // If no curves just draw name on the straight segment
            if (numCurves == 0) { 
                width = find_distance_between_two_points(std::make_pair(fromPosLL, toPosLL));
                angle = getAngle(fromPos, toPos, wasFlipped);
                rend->set_text_rotation(angle);
                center = ezgl::point2d(((toPos.x + fromPos.x) * 0.5), (toPos.y + fromPos.y) * 0.5);
                // If not oneway we don't have to add arrows
                if (!oneWay) {
                    // Using do while to shrink name text size until its drawn or too small
                    do {
                        rend->set_font_size(textHeight);
                            wasDrawn = rend->draw_text(center, streetName, width * 0.8, height);
                        textHeight -= 1.0;
                        } while (!wasDrawn && textHeight >= minTextHeight);
                    }
                // Adds one way arrows to the name being drawn
                else {
                    std::string streetNameArrows; // String containing name and arrows
                    if (wasFlipped) 
                        streetNameArrows = addArrows(streetName, 1, leftArrow);
                    else
                        streetNameArrows = addArrows(streetName, 1, rightArrow);
                    // Using do while to shrink name text size until its drawn or too small
                    do {
                        rend->set_font_size(textHeight);
                        wasDrawn = rend->draw_text(center, streetNameArrows, width, height);
                        textHeight -= 1.0;
                    } while (!wasDrawn && textHeight >= minTextHeight);
                }
            }
            // If segment curves then find largest mostly straight part and draw there
            else {
                double maxWidth = 0, avgAngle = 0;
                ezgl::point2d maxPoint(0, 0);
                bool firstPair = true;
                // Map used to sort the curve points based on distance
                std::map<double, std::pair<double, ezgl::point2d> > distAndAngleFromInt;
                
                // Finds the angle and distance of each curve point from the from intersection
                for (unsigned curveIndex = 0; curveIndex < numCurves; ++curveIndex) {
                    curvePosLL = getStreetSegmentCurvePoint(curveIndex, SSIndex.first);
                    curvePos = ezgl::point2d(xFromLon(curvePosLL.lon()), yFromLat(curvePosLL.lat()));
                    
                    width = find_distance_between_two_points(std::make_pair(fromPosLL, curvePosLL));
                    angle = getAngle(fromPos, curvePos);
                    
                    distAndAngleFromInt.insert(std::make_pair(width, std::make_pair(angle, curvePos)));
                }
                width = find_distance_between_two_points(std::make_pair(fromPosLL, toPosLL));
                angle = getAngle(fromPos, toPos);
                // Adds distance and angle from the from int to the curve point to the map
                distAndAngleFromInt.insert(std::make_pair(width, std::make_pair(angle, toPos)));
                
                // Loops over the map and finds the largest mostly straight part of the map
                auto lastItr = distAndAngleFromInt.begin();
                for (auto itr = distAndAngleFromInt.begin(); itr != distAndAngleFromInt.end(); ++itr) {
                    if (firstPair) { // If its the first curve point use that data as a baseline
                        maxWidth = itr->first;
                        avgAngle = itr->second.first;
                        maxPoint = itr->second.second;
                        firstPair = false;
                        lastItr = itr;
                    }
                    else {
                        curvePos = lastItr->second.second;
                        toPos = itr->second.second;
                        angle = getAngle(curvePos, toPos, wasFlipped);
                        
                        // If the curve point is mostly point the same way as the rest add it
                        if (abs(avgAngle - angle) <= 5.0) {
                            maxWidth = itr->first;
                            avgAngle = (avgAngle + angle) * 0.5;
                            maxPoint = toPos;
                        }
                        else {
                            break;
                        }
                    }
                }
                // Draws text on the largest section that is mostly straight
                rend->set_text_rotation(avgAngle);
                center = ezgl::point2d((maxPoint.x + fromPos.x) * 0.5, (maxPoint.y + fromPos.y) * 0.5);
                if (!oneWay) { // If not one way then we don't need to add one way arrows
                    // Using do while to shrink name text size until its drawn or too small
                    do {
                    rend->set_font_size(textHeight);
                    wasDrawn = rend->draw_text(center, streetName, maxWidth * 0.8, height);
                    textHeight -= 1.0;
                    } while (!wasDrawn && textHeight >= minTextHeight);
                }
                else { // One way so we need to add one way arrows
                    std::string streetNameArrows;
                    if (wasFlipped) 
                        streetNameArrows = addArrows(streetName, 1, leftArrow);
                    else
                        streetNameArrows = addArrows(streetName, 1, rightArrow);
                    // Using do while to shrink name text size until its drawn or too small
                    do {
                        rend->set_font_size(textHeight);
                        wasDrawn = rend->draw_text(center, streetNameArrows, maxWidth, height);
                        textHeight -= 1.0;
                    } while (!wasDrawn && textHeight >= minTextHeight);
                } 
            }
        }
    }
}

// Draws all features besides buildings based on area then draws streams on top
void drawAllFeatures(ezgl::renderer* rend, const double& pixelsPerMeter) {
    drawFeatures(rend);
    drawStreams(rend, pixelsPerMeter);
}

// Draws all features besides buildings and streams
void drawFeatures(ezgl::renderer* rend) {
    std::multimap<double, unsigned> areaOfFeatures = gData.getAreaOfFeatures();
    
    // Iterates backwards to draw from largest to smallest feature
    for (auto itr = areaOfFeatures.rbegin(); itr != areaOfFeatures.rend(); ++itr) {
        rend->set_color(getFeatureColour(getFeatureType((itr->second))));
        LatLon pointLL;
        std::vector<ezgl::point2d> points;
        unsigned numPoints = getFeaturePointCount(itr->second);
        // Loops over all points and calculates/stores their (x,y) values
        for (unsigned point = 0; point < numPoints; ++point) {
            pointLL = getFeaturePoint(point, itr->second);
            points.push_back(ezgl::point2d(xFromLon(pointLL.lon()), yFromLat(pointLL.lat())));
        }
        // Draws features if its largest then one singular point
        if (points.size() > 1) {
            // If its a closed feature draw it as a polygon
            if (points.front() == points.back())
                rend->fill_poly(points);
            // Else draw it as a line
            else
                for (unsigned i = 0; i < points.size() - 1; ++i) 
                    rend->draw_line(points[i], points[i + 1]);
        }
    }
}

// Draws streams separately since they're always on top
void drawStreams(ezgl::renderer* rend, const double& pixelsPerMeter) {
    // OSM States that for water to be a stream its width should be jumpable by
    // an average person. After testing it was found I can jump around 2.5 meters
    rend->set_line_width(std::floor(pixelsPerMeter * 2.5));
    rend->set_color(getFeatureColour(FeatureType::Stream));
    
    // Loops over streams and draws them
    std::vector<unsigned> streams = gData.getIndexesOfStreams();
    for (const unsigned streamIndex : streams) {    
        LatLon pointLL;
        std::vector<ezgl::point2d> points;
        unsigned numPoints = getFeaturePointCount(streamIndex);
        for (unsigned point = 0; point < numPoints; ++point) {
            pointLL = getFeaturePoint(point, streamIndex);
            points.push_back(ezgl::point2d(xFromLon(pointLL.lon()), yFromLat(pointLL.lat())));
        }
        // Draws streams if its largest then one singular point
        if (points.size() > 1) {
            // If its a closed feature draw it as a polygon
            if (points.front() == points.back())
                rend->fill_poly(points);
            // Else draw it as a line
            else
                for (unsigned i = 0; i < points.size() - 1; ++i) 
                    rend->draw_line(points[i], points[i + 1]);
        }
    }
}

// Draws all buildings regardless of type
void drawAllBuildings(ezgl::renderer* rend) {
    drawBuildings(rend, buildingType::other);
    drawBuildings(rend, buildingType::school);
    drawBuildings(rend, buildingType::hospital);
}

// Draws all buildings of any given building type
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
        // Draws buildings if its largest then one singular point
        if (points.size() > 1) {
            // If its a closed feature draw it as a polygon
            if (points.front() == points.back())
                rend->fill_poly(points);
            // Else draw it as a line
            else
                for (unsigned i = 0; i < points.size() - 1; ++i){ 
                    rend->draw_line(points[i], points[i + 1]);
                }
        }
    }
}

// Draws highlights over intersections that need to be highlighted
void drawHighlightedIntersections(ezgl::renderer* rend, const double& pixelsPerMeter) {
    rend->set_color(ezgl::RED);
    HighlightedData data = gData.getHLData();
    if (!data.highlightedInts.empty()) {
        for (const unsigned intIndex : data.highlightedInts) {
            LatLon pos = getIntersectionPosition(intIndex);
            ezgl::point2d pointPos(xFromLon(pos.lon()), yFromLat(pos.lat()));
            rend->fill_elliptic_arc(pointPos, 1/pixelsPerMeter * 5, 1/pixelsPerMeter * 5, 0, 360);
            std::cout<<"Pixels per meter are"<<pixelsPerMeter<<"\n"; 
        }
    }
}

// Example, replace with relevant indices and icons
void drawPOI30(ezgl::renderer* rend, const buildingType& type){
   std::vector<unsigned> buildings = gData.getIndexesOfBuildingType(type);
    ezgl::surface* test; 
    test = rend->load_png("./libstreetmap/resources/school2 30 30.png");
    if (type == buildingType::school){
       test = rend->load_png("./libstreetmap/resources/school2 30 30.png");
    }
    else if (type == buildingType::hospital){
       test = rend->load_png("./libstreetmap/resources/hospital2 30 30.png"); 
    }

    for (const unsigned buildingIndex : buildings) {
        LatLon maxPoint; 
        double maxLon = std::numeric_limits<double>::min(), minLat = std::numeric_limits<double>::max(); 
        maxPoint = getFeaturePoint(0, buildingIndex); 
        for (unsigned curvePoint = 0; curvePoint < getFeaturePointCount(buildingIndex); curvePoint++){
             LatLon pointLL = getFeaturePoint(curvePoint, buildingIndex);
             if(pointLL.lat() < minLat){
                 minLat = pointLL.lat(); 
             }
             maxLon = std::max(maxLon, pointLL.lon()); 
        }
        
        rend->draw_surface(test, ezgl::point2d(xFromLon(maxPoint.lon()) - 15.0, yFromLat(maxPoint.lat()) + 15.0));
    }
    rend->free_surface(test); 
}

//
void drawPOI50(ezgl::renderer* rend, const buildingType& type){
    std::vector<unsigned> buildings = gData.getIndexesOfBuildingType(type);
    ezgl::surface* test; 
    test = rend->load_png("./libstreetmap/resources/school2 30 30.png");
    if (type == buildingType::school){
       test = rend->load_png("./libstreetmap/resources/school2 50 50.png");
    }
    else if (type == buildingType::hospital){
       test = rend->load_png("./libstreetmap/resources/hospital2 50 50.png"); 
    }

    for (const unsigned buildingIndex : buildings) {
        LatLon maxPoint; 
        double maxLon = std::numeric_limits<double>::min(), minLat = std::numeric_limits<double>::max(); 
        maxPoint = getFeaturePoint(0, buildingIndex); 
        for (unsigned curvePoint = 0; curvePoint < getFeaturePointCount(buildingIndex); curvePoint++){
             LatLon pointLL = getFeaturePoint(curvePoint, buildingIndex);
             if(pointLL.lat() < minLat){
                 minLat = pointLL.lat(); 
             }
             maxLon = std::max(maxLon, pointLL.lon()); 
        }
        
        rend->draw_surface(test, ezgl::point2d(xFromLon(maxPoint.lon()) - 25.0, yFromLat(maxPoint.lat()) + 25.0));
    }
    rend->free_surface(test);  
}

//
void drawPOI70(ezgl::renderer* rend, const buildingType& type){
std::vector<unsigned> buildings = gData.getIndexesOfBuildingType(type);
    ezgl::surface* test; 
    test = rend->load_png("./libstreetmap/resources/school2 70 70.png");
    if (type == buildingType::school){
       test = rend->load_png("./libstreetmap/resources/school2 70 70.png");
    }
    else if (type == buildingType::hospital){
       test = rend->load_png("./libstreetmap/resources/hospital2 70 70.png"); 
    }

    for (const unsigned buildingIndex : buildings) {
        LatLon maxPoint; 
        double maxLon = std::numeric_limits<double>::min(), minLat = std::numeric_limits<double>::max(); 
        maxPoint = getFeaturePoint(0, buildingIndex); 
        for (unsigned curvePoint = 0; curvePoint < getFeaturePointCount(buildingIndex); curvePoint++){
             LatLon pointLL = getFeaturePoint(curvePoint, buildingIndex);
             if(pointLL.lat() < minLat){
                 minLat = pointLL.lat(); 
             }
             maxLon = std::max(maxLon, pointLL.lon()); 
        }
        
        rend->draw_surface(test, ezgl::point2d(xFromLon(maxPoint.lon()) - 15.0, yFromLat(maxPoint.lat()) + 15.0));
    }
    rend->free_surface(test); 
}

//
void PrintTTCVehicleInfo30(ptree &ptRoot, ezgl::renderer* rend) {
    string busName;
    int busID = 0;
    double longitude = 0, latitude = 0;
    ezgl::surface* test = rend->load_png("./libstreetmap/resources/train2 30 30.png");
    
    BOOST_FOREACH(ptree::value_type &featVal, ptRoot.get_child("features")) {
        // "features" maps to a JSON array, so each child should have no name
        
        if ( !featVal.first.empty() )
            throw "\"features\" child node has a name";

        //busName = featVal.second.get<string>("properties.route_name");
        //busID = featVal.second.get<int>("properties.vehicle_id");

        // Get GPS coordinates (stored as JSON array of 2 values)
        // Sanity checks: Only 2 values
        ptree coordinates = featVal.second.get_child("geometry.coordinates");
        if (coordinates.size() != 2)
            throw "Coordinates node does not contain 2 items";
        
        longitude = coordinates.front().second.get_value<double>();
        latitude = coordinates.back().second.get_value<double>();

        rend->draw_surface(test, ezgl::point2d(xFromLon(longitude), yFromLat(latitude)));
        rend->draw_surface(test, ezgl::point2d(-79.4325, 43.6525)); 
        
                // Print bus info
       // cout << "Bus " << busName << " with ID " << busID <<
       //     " is at coordinates: " << longitude << ", " << latitude << endl;
          
    }
    rend->free_surface(test); 
    
}

//
void PrintTTCVehicleInfo50(ptree &ptRoot, ezgl::renderer* rend) {
    string busName;
    double longitude = 0, latitude = 0;
    ezgl::surface* test = rend->load_png("./libstreetmap/resources/train2 50 50.png");
    
    BOOST_FOREACH(ptree::value_type &featVal, ptRoot.get_child("features")) {
        // "features" maps to a JSON array, so each child should have no name
        
        if ( !featVal.first.empty() )
            throw "\"features\" child node has a name";

        busName = featVal.second.get<string>("properties.route_name");
        // int busID = featVal.second.get<int>("properties.vehicle_id");

        // Get GPS coordinates (stored as JSON array of 2 values)
        // Sanity checks: Only 2 values
        ptree coordinates = featVal.second.get_child("geometry.coordinates");
        if (coordinates.size() != 2)
            throw "Coordinates node does not contain 2 items";
        
        longitude = coordinates.front().second.get_value<double>();
        latitude = coordinates.back().second.get_value<double>();

        rend->draw_surface(test, ezgl::point2d(xFromLon(longitude), yFromLat(latitude)));
        rend->draw_surface(test, ezgl::point2d(-79.4325, 43.6525)); 
        
                // Print bus info
        //cout << "Bus " << busName << " with ID " << busID <<
            //" is at coordinates: " << longitude << ", " << latitude << endl;
          
    }
    rend->free_surface(test); 
    
}

// 
void PrintTTCVehicleInfo70(ptree &ptRoot, ezgl::renderer* rend) {
    string busName;
    int busID = 0;
    double longitude = 0, latitude = 0;
    ezgl::surface* test = rend->load_png("./libstreetmap/resources/train2 70 70.png");
    
    BOOST_FOREACH(ptree::value_type &featVal, ptRoot.get_child("features")) {
        // "features" maps to a JSON array, so each child should have no name
        
        if ( !featVal.first.empty() )
            throw "\"features\" child node has a name";

        //busName = featVal.second.get<string>("properties.route_name");
        //busID = featVal.second.get<int>("properties.vehicle_id");

        // Get GPS coordinates (stored as JSON array of 2 values)
        // Sanity checks: Only 2 values
        ptree coordinates = featVal.second.get_child("geometry.coordinates");
        if (coordinates.size() != 2)
            throw "Coordinates node does not contain 2 items";
        
        longitude = coordinates.front().second.get_value<double>();
        latitude = coordinates.back().second.get_value<double>();

        rend->draw_surface(test, ezgl::point2d(xFromLon(longitude), yFromLat(latitude)));
        rend->draw_surface(test, ezgl::point2d(-79.4325, 43.6525)); 
        
                // Print bus info
        //cout << "Bus " << busName << " with ID " << busID <<
            //" is at coordinates: " << longitude << ", " << latitude << endl;
          
    }
    rend->free_surface(test); 
    
    //busID++; 
    
}