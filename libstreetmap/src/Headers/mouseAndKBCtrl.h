#ifndef MOUSEANDKEYBOARDCONTROL_H
#define MOUSEANDKEYBOARDCONTROL_H

//==============================================================================
// File Description: Functions which control what our application does when a 
// users clicks mouse or hits key on keyboard
//==============================================================================

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

// Determines what to do based on where the user clicked on the map
void actOnMousePress(ezgl::application* app, GdkEventButton* event, double x, double y);

// Determines what to do based on which key the user pressed down
void actOnKeyPress(ezgl::application* app, GdkEventKey* event, char* key);

#endif /* MOUSEANDKEYBOARDCONTROL_H */

