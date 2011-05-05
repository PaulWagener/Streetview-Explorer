#include "Settings.h"
#include <stdio.h>

Settings settings;

/**
 * Load settings
 */
Settings::Settings() {
    //Default settings
    horizontal_accuracy = 2;
    vertical_accuracy = 5;
    zoom_level = 2;
    wireframe = false;
    mipmapping = false;

    //Load settings (if they exist)
    FILE *f = fopen("settings.txt", "r");

    if (f) {
        fscanf(f, "vertical-acc = %d\n", &vertical_accuracy);
        fscanf(f, "horizontal-acc = %d\n", &horizontal_accuracy);
        fscanf(f, "zoom-level = %d\n", &zoom_level);
        fscanf(f, "mipmapping = %d\n", (int*) &mipmapping);
        fscanf(f, "wireframe = %d\n", (int*) &wireframe);
        
        //Read in the user defined locations
        struct pano_location l;
        while (fscanf(f, "location = %22[a-zA-Z0-90_-] %99[^\n]\n", (char*) &l.pano_id, (char*) &l.name) == 2) {
            locations.push_back(l);
        }
        fclose(f);
    } else {
        //Default locations
        struct pano_location london = {"zyqr7tEQSPQdsXHDy2V51Q", "Trafalgar Square, London"};
        struct pano_location dedam = {"8iF3P8ZJ6HHUELqglMWeEg", "De Dam, Amsterdam"};
        struct pano_location timessquare = {"BXihG0Q9zNPB9r-lkG-NVg", "Times square, New York"};
        struct pano_location louvre = {"FP_aTaS5WoDk8AhmDfir_w", "The Louvre, Paris"};
        struct pano_location lasvegas = {"7zwIO7_PuEatkrBXhsn8BQ", "The Strip, Las Vegas"};
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

    fprintf(f, "vertical-acc = %d\n", vertical_accuracy);
    fprintf(f, "horizontal-acc = %d\n", horizontal_accuracy);
    fprintf(f, "zoom-level = %d\n", zoom_level);
    fprintf(f, "mipmapping = %d\n", mipmapping);
    fprintf(f, "wireframe = %d\n", wireframe);

    for (unsigned int i = 0; i < locations.size(); i++)
        fprintf(f, "location = %s %s\n", locations[i].pano_id, locations[i].name);

    fclose(f);
}
