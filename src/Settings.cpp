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
        while (fscanf(f, "location = %22[a-zA-Z0-90_-] %99[^\n]\n", (char*) &l.pano_id, (char*) &l.name) == 2) {
            locations.push_back(l);
        }
        fclose(f);
    } else {
        //Default locations
        struct pano_location london = {"zyqr7tEQSPQdsXHDy2V51Q", "Trafalgar Square - London"};
        struct pano_location dedam = {"8iF3P8ZJ6HHUELqglMWeEg", "De Dam - Amsterdam"};
        struct pano_location timessquare = {"BXihG0Q9zNPB9r-lkG-NVg", "Times square - New York"};
        struct pano_location louvre = {"FP_aTaS5WoDk8AhmDfir_w", "The Louvre - Paris"};
        struct pano_location lasvegas = {"7zwIO7_PuEatkrBXhsn8BQ", "The Strip - Las Vegas"};
        locations.push_back(london);
        locations.push_back(dedam);
        locations.push_back(timessquare);
        locations.push_back(louvre);
        locations.push_back(lasvegas);
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
