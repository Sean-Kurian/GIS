#ifndef GTKCONTROL_H
#define GTKCONTROL_H

//==============================================================================
// File Description: Sets up and controls all GTK related data such as buttons,
// drop downs, or loading bars.
//==============================================================================

#include "StreetsDatabaseAPI.h"
#include "globalData.h"
#include "drawMapHelpers.h"

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/callback.hpp"

//Connect Zoom buttons to callback functions
void connectZoomButtons(ezgl::application* app);

//Connect Direction related buttons to callback functions
void connectDirectionButtons(ezgl::application* app);

//Callback function to find directions when direction button is pressed
void findDirections(GtkWidget* directionRequestButton, ezgl::application* app);

//Callback function to toggle on/off the direction panel
void toggleDirectionPanel(GtkWidget* directionPanelButton, ezgl::application* app);

//Callback function to toggle on/off the walk interface
void toggleWalkInterface(GtkWidget* walkToggle, ezgl::application* app);

//Set up drop down menu for map switching
void setUpDropDown(ezgl::application* app);

//Callback function for drop down menu
void dropDownChanged(GtkComboBox* dropDownMenu, gpointer data);

//Displays a loading screen while map switching
GtkWidget* displayMapLoadScreen(ezgl::application* app, GtkWidget* progressBar);

//Destroys the loading screen displayed while map switching
void destroyMapLoadScreen(GtkWidget* dialog);

//Parses the map path to return the map name
std::string parseMapName(std::string newMap);

//Creates a progress bar for the load screen
GtkWidget* createProgressBar();

//Increase progress bar
void increaseProgress(ezgl::application* app, GtkWidget* progress);

//Sets up the walking input interface
void setUpWalkInput(ezgl::application* app);

#endif /* GTKCONTROL_H */

