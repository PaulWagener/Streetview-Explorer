#ifndef EXPLORER_H
#define	EXPLORER_H

#include "Panorama.h"
#include "Player.h"
#include <vector>

using namespace std;

/**
 * Explorer is the main class for the actual 'game'.
 * It
 */
class Explorer {
private:
    //Field to hold the pano_id we should load on startup
    //to bridge the gap between the constructor and the time that there is an OpenGL context active
    char firstPanorama[PANOID_LENGTH+1];

    //Fields to communicate between the (down)loading thread and the OpenGL thread
    char downloadPano[PANOID_LENGTH+1];
    int downloadZoomLevel;
    bool downloading;
    Panorama *downloadedPano;

    int currentMipmapping;
    int currentZoomlevel;

    //When walking
    Panorama *oldClosestPanorama;
    float oldClosestOpacity;

    struct utmPosition referencePoint;

    void loadPanorama(const char *panoid, int zoom_level);
    bool gotPanorama(const char *pano_id, int zoom_level);
    bool gotPanorama(Panorama *p);
    Panorama* getPanoramaById(const char* pano_id);
    Panorama* getClosestPanorama();
    
    vector<Panorama*> panoramas;
    void updatePanoramas();

    bool glInitialized;
    
public:
    Player player;

    Explorer(const char* firstPano);
    ~Explorer();
    
    void display(int width, int height);
    void downloadThread();
    bool requestDownloadThread;
};

#endif	/* EXPLORER_H */

