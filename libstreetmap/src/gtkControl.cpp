//==============================================================================
// File Description: Sets up and controls all GTK related data such as buttons,
// drop downs, or loading bars.
//==============================================================================

#include "gtkControl.h"
#include "application.hpp"

#include <regex>

//Connect Zoom buttons to callback functions
void connectZoomButtons(ezgl::application* app) {
    GtkWidget* zoomInButton = GTK_WIDGET(app->get_object("zoomInButton"));
    GtkWidget* zoomOutButton = GTK_WIDGET(app->get_object("zoomOutButton"));
    GtkWidget* zoomFitButton = GTK_WIDGET(app->get_object("zoomFitButton"));
    
    g_signal_connect(G_OBJECT(zoomInButton), "clicked", G_CALLBACK(ezgl::press_zoom_in), app);
    g_signal_connect(G_OBJECT(zoomOutButton), "clicked", G_CALLBACK(ezgl::press_zoom_out), app);
    g_signal_connect(G_OBJECT(zoomFitButton), "clicked", G_CALLBACK(ezgl::press_zoom_fit), app);
}

//Connect Direction related buttons to callback functions
void connectDirectionButtons(ezgl::application* app) {
    GtkWidget* directionPanelButton = GTK_WIDGET(app->get_object("directionPanelButton"));
    GtkWidget* collapseButton = GTK_WIDGET(app->get_object("collapseButton"));
    GtkWidget* directionRequestButton = GTK_WIDGET(app->get_object("directionRequestButton"));
    GtkWidget* walkToggleButton = GTK_WIDGET(app->get_object("walkToggle"));
    
    
    g_signal_connect(G_OBJECT(directionPanelButton), "clicked", G_CALLBACK(showDirectionPanel), app);
    g_signal_connect(G_OBJECT(collapseButton), "clicked", G_CALLBACK(collapseDirectionPanel), app);
    g_signal_connect(G_OBJECT(directionRequestButton), "clicked", G_CALLBACK(findDirections), app);
    g_signal_connect(G_OBJECT(walkToggleButton), "toggled", G_CALLBACK(toggleWalkInterface), app);
    
    //Hide the direction panel to start 
    GtkWidget* directionPanel = GTK_WIDGET(app->get_object("DirectionPanel"));
    GtkWidget* bottomDirectionPanel = GTK_WIDGET(app->get_object("bottomDirectionPanel"));
    gtk_widget_hide(directionPanel);
    gtk_widget_hide(bottomDirectionPanel);
}

//Callback function to find directions when direction button is pressed
void findDirections(GtkWidget* directionRequestButton, ezgl::application* app) {
    //Determine the start and destination intersections being searched for
    GtkEntry* startSearchEntry = (GtkEntry*) app->get_object("searchBar");
    GtkEntry* destinationSearchEntry = (GtkEntry*) app->get_object("secondSearchBar");
    std::string startSearch = gtk_entry_get_text(startSearchEntry);
    std::string destinationSearch = gtk_entry_get_text(destinationSearchEntry);
    
    //Ensure intersections are being searched for (each entry has an "&" or "and")
    if ((startSearch.find("&") != std::string::npos || startSearch.find("and") != std::string::npos) &&
            (destinationSearch.find("&") != std::string::npos || destinationSearch.find("and") != std::string::npos)) {
        //Format the searches
        startSearch = parseIntersectionSearch(startSearch);
        destinationSearch = parseIntersectionSearch(destinationSearch);
        
        //Find the intersection indexes
        IntersectionIndex startIndex = find_intersection_from_name(startSearch);
        IntersectionIndex destinationIndex = find_intersection_from_name(destinationSearch);
        
        //Alert users if any intersections are not found
        if (startIndex == -1) {
            std::cout << "Starting intersection not found\n";
        }
        else if (destinationIndex == -1) {
            std::cout << "Destination intersection not found\n";
        }
        
        //Otherwise, determine a path between two intersections
        // This is where find path would go
        else {
            std::cout << "Starting Intersection: " << getIntersectionName(startIndex) << "\n";
            std::cout << "Destination Intersection: " << getIntersectionName(destinationIndex) << "\n";
        }
    }
    
    //Otherwise, display error message (later step could be splitting it to determine which entry contains the error)
    else {
        std::cout << "Invalid search entered\n";
    }
}

//Callback function to show direction panel
void showDirectionPanel(GtkWidget* directionPanelButton, ezgl::application* app) {
    GtkWidget* directionPanel = GTK_WIDGET(app->get_object("DirectionPanel"));
    GtkWidget* bottomDirectionPanel = GTK_WIDGET(app->get_object("bottomDirectionPanel"));
    gtk_widget_show(directionPanel);
    gtk_widget_show(bottomDirectionPanel);
    
    //Hide the main search bar and direction panel button
    GtkWidget* mainSearchBar = GTK_WIDGET(app->get_object("mainSearchBar"));
    gtk_widget_hide(directionPanelButton);
    gtk_widget_hide(mainSearchBar);
}

//Callback function to collapse the direction panel
void collapseDirectionPanel(GtkWidget* collapseButton, ezgl::application* app) {
    GtkWidget* directionPanel = GTK_WIDGET(app->get_object("DirectionPanel"));
    GtkWidget* bottomDirectionPanel = GTK_WIDGET(app->get_object("bottomDirectionPanel"));
    gtk_widget_hide(directionPanel);
    gtk_widget_hide(bottomDirectionPanel);
    
    //Show the main search bar and direction panel button
    GtkWidget* directionPanelButton = GTK_WIDGET(app->get_object("directionPanelButton"));
    GtkWidget* mainSearchBar = GTK_WIDGET(app->get_object("mainSearchBar"));
    gtk_widget_show(directionPanelButton);
    gtk_widget_show(mainSearchBar);
}

//Callback function to toggle on/off the walk interface
void toggleWalkInterface(GtkWidget* walkToggleButton, ezgl::application* app) {
    //Get the grid that holds the walk input interface
    GtkWidget* walkInputGrid = GTK_WIDGET(app->get_object("walkInputGrid"));
    
    //Determine if the interface needs to shown or hidden
    if (gtk_toggle_button_get_active((GtkToggleButton*) walkToggleButton)) {
        gtk_widget_show(walkInputGrid);
    } else {
        gtk_widget_hide(walkInputGrid);
    }
}

//Set up drop down menu for map switching
void setUpDropDown(ezgl::application* app) {
    GtkComboBox* dropDownMenu = (GtkComboBox*) app->get_object("mapDropDown");
    GtkWidget* switchMapButton = GTK_WIDGET(app->get_object("switchMapButton"));
    
    //Set the default selection to be the current map
    std::string mapPath = gData.getMapPath();
    const char* mapPathCstr = mapPath.c_str();
    gtk_combo_box_set_active_id(dropDownMenu, mapPathCstr);
    
    g_signal_connect(G_OBJECT(switchMapButton), "clicked", G_CALLBACK(switchMap), app);
}

//Displays a loading screen while map switching
GtkWidget* displayMapLoadScreen(ezgl::application* app, GtkWidget* progressBar) {
    //Determine the map being loaded, and create a loading message
    GtkComboBox* dropDownMenu = (GtkComboBox*) app->get_object("mapDropDown");
    const gchar* map_id = gtk_combo_box_get_active_id(dropDownMenu);
    std::string load_message = "Now loading: " + parseMapName(map_id);
    
    //Create the loading screen
    GObject* window;
    GtkWidget* content_area;
    GtkWidget* label;
    GtkWidget* dialog;
    GtkWidget* grid;

    window = app->get_object(app->get_main_window_id().c_str());
    grid = gtk_grid_new();
    dialog = gtk_dialog_new_with_buttons(
            "Loading",
            (GtkWindow*) window,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            NULL, NULL);
    
    //Add the content to the loading screen
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    label = gtk_label_new(load_message.c_str());
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_attach(GTK_GRID(grid), progressBar, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_container_add(GTK_CONTAINER(content_area), grid);
    
    //Display the loading screen
    gtk_widget_show_all(dialog);
    app->refresh_drawing();
    app->flush_drawing();
    
    return dialog;
}

//Destroys the loading screen displayed while map switching
void destroyMapLoadScreen(GtkWidget* dialog) {
    gtk_widget_destroy(dialog);
}

//Parses the map path to return the map name
std::string parseMapName(std::string newMap) {
    const int MAP_NAME_BEGINS = 26;
    const int MAP_NAME_ENDS = newMap.find(".");
    const int NEXT_INDEX = 1;
    newMap = newMap.substr(MAP_NAME_BEGINS, MAP_NAME_ENDS - MAP_NAME_BEGINS);
    
    std::regex dash("-");
    std::regex underscore("_");
    newMap = std::regex_replace(newMap, dash, " ");
    newMap = std::regex_replace(newMap, underscore, ", ");

    newMap[0] = toupper(newMap[0]);
    int capitalIndex = newMap.find(" ") + NEXT_INDEX;
    while (capitalIndex != 0) {
        newMap[capitalIndex] = toupper(newMap[capitalIndex]);
        capitalIndex = newMap.find(" ", capitalIndex) + NEXT_INDEX;
    }
    return newMap;
}

//Creates a progress bar for the load screen
GtkWidget* createProgressBar() {
    const double THIRD = 0.33;
    GtkWidget* progressBar = gtk_progress_bar_new();
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), THIRD);
    return progressBar;
}
//Increase progress bar
void increaseProgress(ezgl::application* app, GtkWidget* progressBar) {
    const double THIRD = 0.33;
    gdouble progress = gtk_progress_bar_get_fraction(GTK_PROGRESS_BAR(progressBar)) + THIRD;
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), progress);
    
    app->flush_drawing();
}

//Sets up the walking input interface
void setUpWalkInput(ezgl::application* app) {
    //Right justify the numerical inputs
    GtkEntry* walkingSpeedEntry = (GtkEntry*) app->get_object("walkingSpeed");
    GtkEntry* walkingLimitEntry = (GtkEntry*) app->get_object("walkingLimit");
    
    gtk_entry_set_alignment(walkingSpeedEntry, 1);
    gtk_entry_set_alignment(walkingLimitEntry, 1);
    
    //Hide the walk input interface
    GtkWidget* walkInputGrid = GTK_WIDGET(app->get_object("walkInputGrid"));
    gtk_widget_hide(walkInputGrid);
}