#include "mouseAndKBCtrl.h"

void actOnMousePress(ezgl::application* app, GdkEventButton* event, double x, double y) {
    app->update_message("Mouse Clicked");
    std::cout << "User clicked mouse at (" << x << "," << y << ")\n";
    ezgl::renderer* rend = app->get_renderer();
    rend->set_coordinate_system(ezgl::SCREEN);
    std::cout << "User clicked mouse at (" << x << "," << y << ")\n";
}