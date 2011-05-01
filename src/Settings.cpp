/* 
 * File:   Settings.cpp
 * Author: paulwagener
 * 
 * Created on 23 april 2011, 17:11
 */

#include "Settings.h"
#include <stdio.h>
Settings settings;

/**
 * Load settings
 */
Settings::Settings() {
    //Default settings
    horizontal_accuracy = 16;
    vertical_accuracy = 10;
    numPanoramas = 5;
    zoomLevel = 2;
    wireframe = false;
    mipmapping = false;
    last_pano[0] = '\0';

    //Load settings (if they exist)
    FILE *f = fopen("settings.txt", "r");
    if (f) {
        fscanf(f, "last-location = %s\n", (char*)&last_pano);
        fscanf(f, "vertical-acc = %d\n", &vertical_accuracy);
        fscanf(f, "horizontal-acc = %d\n", &horizontal_accuracy);
        fscanf(f, "num-panos = %d\n", &numPanoramas);

        //Read in the user defined locations
        struct pano_location l;
        while (fscanf(f, "location = %22[a-zA-Z0-90_-] %99[^\n]\n", (char*) &l.pano_id, (char*) &l.name) > 0) {
            locations.push_back(l);
        }
        fclose(f);

    }
}

/**
 * Save settings
 */
void Settings::saveSettings() {
    FILE *f = fopen("settings.txt", "w");

    if(last_pano[0] != '\0')
        fprintf(f, "last-location = %s\n", last_pano);

    fprintf(f, "vertical-acc = %d\n", vertical_accuracy);
    fprintf(f, "horizontal-acc = %d\n", horizontal_accuracy);
    fprintf(f, "num-panos = %d\n", numPanoramas);

    for(unsigned int i = 0; i < locations.size(); i++)
        fprintf(f, "location = %s %s\n", locations[i].pano_id, locations[i].name);

    fclose(f);
}
