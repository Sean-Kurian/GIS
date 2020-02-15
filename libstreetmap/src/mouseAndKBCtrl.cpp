#include "mouseAndKBCtrl.h"

void actOnMousePress(ezgl::application* app, GdkEventButton* event, double x, double y) {
    app->update_message("Mouse Clicked");
    std::cout << "User clicked mouse at (" << x << "," << y << ")\n";
}