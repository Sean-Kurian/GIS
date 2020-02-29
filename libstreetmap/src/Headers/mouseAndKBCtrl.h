#ifndef MOUSEANDKEYBOARDCONTROL_H
#define MOUSEANDKEYBOARDCONTROL_H

//==============================================================================
// File Description: Defines events to capture mouse clicks and keyboard presses
//
//==============================================================================

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

//
void actOnMousePress(ezgl::application* app, GdkEventButton* event, double x, double y);

//
void actOnKeyPress(ezgl::application* app, GdkEventKey* event, char* key);


#endif /* MOUSEANDKEYBOARDCONTROL_H */

