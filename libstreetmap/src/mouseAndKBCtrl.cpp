#include "mouseAndKBCtrl.h"
#include "globalData.h"
#include "m1.h"
#include "drawMapHelpers.h"
#include "LatLon.h"
#include "displayInfo.h"


void actOnMousePress(ezgl::application* app, GdkEventButton* event, double x, double y) {
    app->update_message("Mouse Clicked");
    std::cout << "User clicked mouse at (" << x << "," << y << ")\n";
    LatLon clicked = LatLon(latFromY(y), lonFromX(x));
    
    //If it is a left click, display intersection info
    if (event->button == 1) {
        unsigned intIndex = find_closest_intersection(clicked);
        std::cout << "Closest intersection: " << intIndex << "\n";
        gData.addHighlightedInt(intIndex);
        displayIntersectionInfo(app, intIndex);
        app -> refresh_drawing(); 
        gData.removeLastHighlightedInt(); 
    }
    
}