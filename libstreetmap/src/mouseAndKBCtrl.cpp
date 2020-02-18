#include "mouseAndKBCtrl.h"
#include "m1.h"
#include "drawMapHelpers.h"
#include "LatLon.h"


void actOnMousePress(ezgl::application* app, GdkEventButton* event, double x, double y) {
    app->update_message("Mouse Clicked");
    std::cout << "User clicked mouse at (" << x << "," << y << ")\n";
    LatLon clicked = LatLon(latFromY(y), lonFromX(x));
    std::cout << "Closest intersection: " << find_closest_intersection(clicked) << "\n";
}