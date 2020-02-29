//==============================================================================
// File Description: Defines events to capture mouse clicks and keyboard presses
//
//==============================================================================

#include "mouseAndKBCtrl.h"
#include "globalData.h"
#include "m1.h"
#include "drawMapHelpers.h"
#include "colourFunctions.h"
#include "LatLon.h"
#include "displayInfo.h"


void actOnMousePress(ezgl::application* app, GdkEventButton* event, double x, double y) {
    //app->update_message("Mouse Clicked");
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
    
    //If it is not a left click, erase the displayed intersection info if it exists
    else {
        if (gData.getIntersectionInfoBox() != nullptr) {
            eraseIntersectionInfo(gData.getIntersectionInfoBox());
        }
    }
    
}

//
void actOnKeyPress(ezgl::application* app, GdkEventKey* event, char* key) {
    if (event->type == GdkEventType::GDK_KEY_PRESS) {
        ezgl::canvas* canvas = app->get_canvas(app->get_main_canvas_id());
        if (strcmp(key, "Up") == 0)
            ezgl::translate_up(canvas, 5.0);
        else if (strcmp(key, "Down") == 0)
            ezgl::translate_down(canvas, 5.0);
        else if (strcmp(key, "Left") == 0)
            ezgl::translate_left(canvas, 5.0);
        else if (strcmp(key, "Right") == 0)
            ezgl::translate_right(canvas, 5.0);
        else if (strcmp(key, "n") || strcmp(key, "N")) {
            nightMode::isOn = !nightMode::isOn;
            canvas->redraw();
        }
    }
}