#ifndef DRAWCUSTOMBUTTONS_H
#define DRAWCUSTOMBUTTONS_H

#include "StreetsDatabaseAPI.h"
#include "globalData.h"
#include "gtkObjects.h"
#include "drawMapHelpers.h"

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/callback.hpp"

//Connect Zoom buttons to callback functions
void connectZoomButtons(ezgl::application* app);

//Set up drop down menu for map switching
void setUpDropDown(ezgl::application* app);

//Callback function for drop down menu
void dropDownChanged(GtkComboBox* dropDownMenu, gpointer data);

#endif /* DRAWCUSTOMBUTTONS_H */

