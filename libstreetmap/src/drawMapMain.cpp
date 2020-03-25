//==============================================================================
// File Description: Loads and draws maps, as well as draws features at various zoom levels 
// as defined by the size of pixelsPerMeter. Zoom levels were chosen based on optimal sizing. 
//==============================================================================

#include "m1.h"
#include "m2.h"
#include "StreetsDatabaseAPI.h"
#include "globalData.h"
#include "drawMapObjects.h"
#include "drawMapHelpers.h"
#include "gtkControl.h"
#include "mouseAndKBCtrl.h"
#include "colourFunctions.h"
#include "searchBar.h"
#include "displayInfo.h"
#include "libcurl.h"

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

#include <iostream>
#include <ctime>

// Forward declarations of draw map sub functions
void drawMainCanvas(ezgl::renderer* rend);
double pixelInMeters(ezgl::renderer* rend);
void initialSetup(ezgl::application* app, bool newWindow);
ptree getRoot(); 

// Controls the drawing of our map
void draw_map() {
    // Initial setup for our application
    ezgl::application::settings settings;
    settings.main_ui_resource = "libstreetmap/resources/main.ui";
    settings.window_identifier = "MainWindow"; 
    settings.canvas_identifier = "MainCanvas";  
    ezgl::application application(settings); 
    
    // Finds the min and max points on our map based on their lat/lon
    std::cout << "MinLat / MaxLat: " << gData.getMinLat() << " " << gData.getMaxLat() << "\n";
    std::cout << "MinLon / MaxLon: " << gData.getMinLon() << " " << gData.getMaxLon() << "\n";
    ezgl::point2d minPt(xFromLon(gData.getMinLon()), yFromLat(gData.getMinLat()));
    ezgl::point2d maxPt(xFromLon(gData.getMaxLon()), yFromLat(gData.getMaxLat()));
    std::cout << "Min point: " << minPt.x << " " << minPt.y << "\n";
    std::cout << "Max point: " << maxPt.x << " " << maxPt.y << "\n";
    // Rectangle that is the size of our entire map
    ezgl::rectangle mapCoords{ minPt, maxPt };
    
    // Creates our main canvas which will be used to draw the map on and connects callback
    application.add_canvas("MainCanvas", drawMainCanvas, mapCoords);
    
    // Starts application and connects all callback functions
    application.run(initialSetup, actOnMousePress, nullptr, actOnKeyPress);
}

//
void drawMainCanvas(ezgl::renderer* rend) {
    // Changes background colour based on time of day
    rend->set_color(nightMode::isOn ? NIGHT_BACKGROUND_COLOUR : DAY_BACKGROUND_COLOUR);
    rend->fill_rectangle(rend->get_visible_world());
    rend->set_line_cap(ezgl::line_cap::round);
    ptree ptRoot = getRoot(); 
    
    // Gets the pixels per meter to determine how zoomed in user is
    double pixelsPerMeter = pixelInMeters(rend);
    // Draws all features in order from largest to smallest
    drawFeatures(rend);
    
    // Chooses what to draw based on how zoomed in the user is
    if (pixelsPerMeter < 0.01) {
        drawStreets(rend, roadType::highway, pixelsPerMeter);
    }
    
    else if (pixelsPerMeter < 0.1) {
        drawStreams(rend, pixelsPerMeter);
        
        drawStreets(rend, roadType::majorRoad, pixelsPerMeter);
        drawStreets(rend, roadType::highway, pixelsPerMeter);
    }
    
    else if (pixelsPerMeter < 0.2) {
        drawStreams(rend, pixelsPerMeter);
        
        drawStreets(rend, roadType::minorRoad, pixelsPerMeter);
        drawStreets(rend, roadType::majorRoad, pixelsPerMeter);
        drawStreets(rend, roadType::highway, pixelsPerMeter);
    }
    
    else if (pixelsPerMeter < 0.4) {
        drawStreams(rend, pixelsPerMeter);
        
        drawBuildings(rend, buildingType::school);
        drawBuildings(rend, buildingType::hospital);
        
        drawStreets(rend, roadType::minorRoad, pixelsPerMeter);
        drawStreets(rend, roadType::majorRoad, pixelsPerMeter);
        drawStreets(rend, roadType::highway, pixelsPerMeter);
        
        drawPOI30(rend, buildingType::school);
        drawPOI30(rend, buildingType::hospital);
        PrintTTCVehicleInfo30(ptRoot, rend); 
        
    }
    else if (pixelsPerMeter < 0.75) {
        drawStreams(rend, pixelsPerMeter);
        
        drawBuildings(rend, buildingType::school);
        drawBuildings(rend, buildingType::hospital);
        drawBuildings(rend, buildingType::other);
        
        drawStreets(rend, roadType::minorRoad, pixelsPerMeter);
        drawStreets(rend, roadType::majorRoad, pixelsPerMeter);
        drawStreetNames(rend, roadType::majorRoad, pixelsPerMeter);
        drawStreets(rend, roadType::highway, pixelsPerMeter);
        drawStreetNames(rend, roadType::highway, pixelsPerMeter);
        
        drawPOI50(rend, buildingType::school);
        drawPOI50(rend, buildingType::hospital);
        PrintTTCVehicleInfo50(ptRoot, rend); 
    }
    
    else {
        drawStreams(rend, pixelsPerMeter);
        
        drawBuildings(rend, buildingType::school);
        drawBuildings(rend, buildingType::hospital);
        drawBuildings(rend, buildingType::other);
        
        drawPaths(rend, pixelsPerMeter);
        drawStreets(rend, roadType::minorRoad, pixelsPerMeter);
        drawStreetNames(rend, roadType::minorRoad, pixelsPerMeter);
        drawStreets(rend, roadType::majorRoad, pixelsPerMeter);
        drawStreetNames(rend, roadType::majorRoad, pixelsPerMeter);
        drawStreets(rend, roadType::highway, pixelsPerMeter);
        drawStreetNames(rend, roadType::highway, pixelsPerMeter);
        
        drawPOI70(rend, buildingType::school);
        drawPOI70(rend, buildingType::hospital);
        PrintTTCVehicleInfo70(ptRoot, rend); 
    }
    drawHighlightedStreets(rend, pixelsPerMeter);
    drawHighlightedIntersections(rend);
}

// Calculates the number of pixels in a given
double pixelInMeters(ezgl::renderer* rend) {
    ezgl::rectangle world = rend->get_visible_world();
    ezgl::rectangle oneMeterBox(world.center(), 1, 1);
    ezgl::rectangle screen1Meter = rend->world_to_screen(oneMeterBox);
    std::cout << "1 meter in pixels: " << screen1Meter.width() << "\n";
    return screen1Meter.width();
}

//Initial setup of the application
void initialSetup(ezgl::application* app, bool) {
    // Determines if it's nighttime and if it is it enables night mode
    time_t now = time(0);
    tm* localTime = localtime(&now);
    if (localTime->tm_hour > 19 || localTime->tm_hour < 7)
        nightMode::isOn = true;
    else
        nightMode::isOn = false;
    
    // Sets up and connects GTK objects to our map
    connectZoomButtons(app);
    connectSearchBar(app);
    connectDirectionButtons(app);
    setUpDropDown(app);
    setUpWalkInput(app);
}

// Does necessary setup for libcurl
static size_t writeData(void *buffer, size_t size, size_t nmemb, void *userp) {
    if (buffer && nmemb && userp) {
        MyCustomStruct *pMyStruct = (MyCustomStruct *)userp;

        // Writes to struct passed in from main
        if (pMyStruct->response == nullptr) {
            // Case when first time write_data() is invoked
            pMyStruct->response = new char[nmemb + 1];
            strncpy(pMyStruct->response, (char *)buffer, nmemb);
        }
        else {
            // Case when second or subsequent time write_data() is invoked
            char *oldResp = pMyStruct->response;

            pMyStruct->response = new char[pMyStruct->size + nmemb + 1];

            // Copy old data
            strncpy(pMyStruct->response, oldResp, pMyStruct->size);

            // Append new data
            strncpy(pMyStruct->response + pMyStruct->size, (char *)buffer, nmemb);

            delete [] oldResp;
        }
        pMyStruct->size += nmemb;
        pMyStruct->response[pMyStruct->size] = '\0';
    }
    return nmemb;
}

// Loads JSON file and parses it for data
ptree getRoot(){
    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
    if (res != CURLE_OK) {
        cout << "ERROR: Unable to initialize libcurl" << endl;
        cout << curl_easy_strerror(res) << endl;
    }
    ptree ptRoot; 
    CURL *curlHandle = curl_easy_init();
    if ( !curlHandle ) {
        cout << "ERROR: Unable to get easy handle" << endl;
    } else {
        char errbuf[CURL_ERROR_SIZE] = {0};
        MyCustomStruct myStruct;
        char targetURL[] = "http://portal.cvst.ca/api/0.1/ttc/geojson";

        res = curl_easy_setopt(curlHandle, CURLOPT_URL, targetURL);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_ERRORBUFFER, errbuf);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, writeData);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &myStruct);

        myStruct.url = targetURL;

        if (res != CURLE_OK) {
            cout << "ERROR: Unable to set libcurl option" << endl;
            cout << curl_easy_strerror(res) << endl;
        } 
        else {
            res = curl_easy_perform(curlHandle);
        }

        cout << endl << endl;
        if (res == CURLE_OK) {
            // Create an empty proper tree
            istringstream issJsonData(myStruct.response);
            read_json(issJsonData, ptRoot);
        } 
        else {
            cout << "ERROR: res == " << res << endl;
            cout << errbuf << endl;
        }
        if (myStruct.response)
            delete []myStruct.response;

        curl_easy_cleanup(curlHandle);
        curlHandle = nullptr;
    }
    curl_global_cleanup();
    return ptRoot; 
}



