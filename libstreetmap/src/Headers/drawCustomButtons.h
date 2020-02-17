/* 
 * File:   drawCustomButtons.h
 * Author: green107
 * Header file for functions used to draw custom buttons on top of map
 * Created on February 17, 2020, 3:21 PM
 */

#ifndef DRAWCUSTOMBUTTONS_H
#define DRAWCUSTOMBUTTONS_H

#include "StreetsDatabaseAPI.h"
#include "globalData.h"

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

//Draws the Zoom Buttons on top of map 
void drawZoomButtons(ezgl::renderer* rend);

#endif /* DRAWCUSTOMBUTTONS_H */

