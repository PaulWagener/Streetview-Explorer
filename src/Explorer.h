/* 
 * File:   Explorer.h
 * Author: paulwagener
 *
 * Created on 12 april 2011, 19:24
 */

#include "Panorama.h"
#include "Player.h"

#ifndef EXPLORER_H
#define	EXPLORER_H

class Explorer {
public:
    Explorer(const char* firstPano);

    char firstPanorama[PANOID_LENGTH+1];
    bool requestDownloadThread;
    
    char downloadPano[PANOID_LENGTH+1];
    bool downloading;
    Panorama *downloadedPano;

    bool hasPanorama(const char *pano_id, int zoom_level);
    bool hasPanorama(Panorama *p);
    Panorama* getClosestPanorama();
    
    std::vector<Panorama*> panoramas;
    void updatePanoramas();
    Panorama* getPanoramaById(const char* pano_id);

    Panorama *oldClosestPanorama;

    float oldClosestOpacity;

    Player player;

    void downloadThread();

    struct utmPosition referencePoint;
    void display(int width, int height);

    void loadPanorama(const char *panoid, int zoom_level);
    void collision_detection();
    void init();
private:

};

#endif	/* EXPLORER_H */

