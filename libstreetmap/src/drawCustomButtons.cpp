/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ezgl/graphics.hpp"


#include "drawCustomButtons.h"

//Draws Zoom Buttons on top of map
void drawZoomButtons(ezgl::renderer* rend) {
    rend->set_coordinate_system(ezgl::SCREEN);
    
    
    rend->set_coordinate_system(ezgl::WORLD);
}
