#ifndef SETTINGS_H
#define	SETTINGS_H

#include <vector>
#include "Panorama.h"
#define LOCATION_LENGTH 100

struct pano_location {
    char pano_id[23];
    char name[LOCATION_LENGTH+1];
};

class Settings {
public:
    int mipmapping;
    int wireframe;
    int horizontal_accuracy;
    int vertical_accuracy;
    int zoom_level;

    std::vector<struct pano_location> locations;

    Settings();
    void saveSettings();
private:

};

extern Settings settings;

#endif	/* SETTINGS_H */

