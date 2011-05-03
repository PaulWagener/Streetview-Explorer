/* 
 * File:   Explorer.cpp
 * Author: paulwagener
 * 
 * Created on 12 april 2011, 19:24
 */

#include "Explorer.h"
#include "gl.h"
#include "stdio.h"
#include "common.h"
#include "Panorama.h"
#include "Settings.h"
#include <math.h>

Explorer::Explorer(const char* firstPano) {
    requestDownloadThread = false;
    downloading = false;
    downloadedPano = NULL;

    oldClosestPanorama = NULL;
    oldClosestOpacity = 1;

    //Temporarily store the first panorama we should load
    //until we have an OpenGL context to actually load them in
    strncpy((char*) &firstPanorama, firstPano, PANOID_LENGTH + 1);
}

/**
 * Load a new panorama into the panoramas vector.
 * It needs to be called twice if the panorama isn't donloaded yet
 * Assumes an OpenGL context to be present
 * 
 * @param panoid
 * @param zoom_level
 */
void Explorer::loadPanorama(const char *panoid, int zoom_level) {
    if (downloading)
        return;

    //File is not cached, request a download procedure
    strncpy(downloadPano, panoid, PANOID_LENGTH + 1);
    downloading = true;
    requestDownloadThread = true;
    //downloadThread();

}

void Explorer::init() {
    //Kruisstraat (kruising): FKoscBO7bd5LkgMmjGLAeQ
    //Kruisstraat (weverstraat): 6B46Si-orLNYnBhDKdPDiw
    //Kruisstraat (weverstraat 2): eE1uAIWbo1bDccxVNyE0Jg
    //Kruisstraat (weverstraat 3): cCstFkrrJxSI5Ql4OOPb1Q
    //Kruisstraat (weverstraat 4): GL6UZFuHTQLQCnAvE2VTHw
    //Kruisstraat (115): qdAqxSu085_gynN0R8k4RA
    //Times Square: dU1D9CsdYTN-3YDiyyUSnQ
}

/**
 *
 */
void Explorer::downloadThread() {
	try {
        Panorama *p = new Panorama(downloadPano, settings.zoomLevel);
        downloadedPano = p;
    } catch (const char* c) {
        printf("Exception caught: %s\n", c);
    }
    downloading = false;
}

bool Explorer::hasPanorama(const char* pano_id, int zoom_level) {
    for (unsigned int i = 0; i < panoramas.size(); i++) {
        if (strcmp(panoramas[i]->pano_id, pano_id) == 0 && panoramas[i]->zoom_level == zoom_level)
            return true;
    }
    return false;
}

bool Explorer::hasPanorama(Panorama *panorama) {
    for (unsigned int i = 0; i < panoramas.size(); i++) {
        if (panoramas[i] == panorama)
            return true;
    }
    return false;
}

Panorama* Explorer::getClosestPanorama() {
    if (panoramas.size() == 0)
        return NULL;

    Panorama *closestPanorama = panoramas[0];
    for (unsigned int i = 0; i < panoramas.size(); i++) {
        if (panoramas[i]->distanceTo(player.target_location) < closestPanorama->distanceTo(player.target_location))
            closestPanorama = panoramas[i];
    }
    return closestPanorama;


}
bool efirst = true;

/**
 * 
 */
void Explorer::updatePanoramas() {
    settings.numPanoramas = 5;
    if (downloading)
        return;

    //Load thread got a new panorama for us, add it to the official panorama list
    if (downloadedPano != NULL) {
        downloadedPano->loadGL();
        panoramas.push_back(downloadedPano);

        if (panoramas.size() == 1) {
            referencePoint = downloadedPano->location;
            player.initializeLocation(downloadedPano->location);
        }

        downloadedPano = NULL;
    }

    Panorama *closestPanorama = getClosestPanorama();
    //If no panorama's have yet been downloaded load the panorama that was given in the constructor
    if (panoramas.size() == 0) {
        loadPanorama(firstPanorama, settings.zoomLevel);
    } else if (closestPanorama != NULL) {

        //First delete all panorama's that are not adjacent to the closest panorama
        //and are not the closestPanorama itself
        for (unsigned int i = 0; i < panoramas.size(); i++) {
            if (panoramas[i] != closestPanorama && !closestPanorama->hasAdjacent(panoramas[i]->pano_id)) {
                panoramas.erase(panoramas.begin() + i);
                i--;
            }
        }

        //Then see if there are any adjacent panorama's not already loaded
        for (unsigned int i = 0; i < closestPanorama->links.size(); i++) {
            if (!hasPanorama(closestPanorama->links[i].pano_id, settings.zoomLevel))
                loadPanorama(closestPanorama->links[i].pano_id, settings.zoomLevel);
        }
    }

}

/**
 * Find a panorama by its pano_id
 * 
 * @param pano_id
 * @return
 */
Panorama* Explorer::getPanoramaById(const char* pano_id) {
    for (unsigned int i = 0; i < panoramas.size(); i++) {
        if (strcmp(pano_id, panoramas[i]->pano_id) == 0)
            return panoramas[i];
    }
    return NULL;
}

int g = 0;

void Explorer::display(int width, int height) {
    updatePanoramas();
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, width / (GLdouble) height, 1, 200.0);

    /**
     * Coordinate system:
     * y+   North
     * y-   South
     * x+   East
     * x-   West
     * z+   Up
     * z-   Down
     * @param w
     * @param h
     */

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* Initialize OpenGL */
    glClearColor(1, 1, 1, 1);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    //Light is only used to illuminate Streetview Guy,
    //The panorama's themselves are drawn at full intensity without lighting.
    glEnable(GL_LIGHT0);

	GLfloat ambient[] =  {0.8f, 0.8f, 0.8f, 1.0f};
	GLfloat diffuse[] =  {0.6f, 0.7f, 0.7f, 1.0f};
	GLfloat specular[] =  {1.0f, 1.0f, 1.0f, 1.0f};	
	GLfloat position[] =  {0.0f, 0.0f, 0.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



    player.updatePosition();

    //Update closestPanorama
    //Below this code we can use closestPanorama


    collision_detection();
    glLoadIdentity();

    player.targetCamera(referencePoint);

    glColor3f(1, 1, 1);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);


    if (settings.wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    Panorama *closestPanorama = getClosestPanorama();

    //Ensure oldClosestPanorama ain't dangling
    if (!hasPanorama(oldClosestPanorama))
        oldClosestPanorama = NULL;

    //Increase opacity of all Panorama's
    for (unsigned int i = 0; i < panoramas.size(); i++) {
        panoramas[i]->opacity += 0.03f;
        if (panoramas[i]->opacity > 1)
            panoramas[i]->opacity = 1;
    }

    /**
     * Draw the background, this is from the panorama the closest
     * to the camera because it has the best perspective
     */
    if (closestPanorama != NULL) {

        glColor4f(1, 1, 1, 1);
        closestPanorama->draw(referencePoint, true);
        glClear(GL_DEPTH_BUFFER_BIT);

        if (oldClosestPanorama == NULL)
            oldClosestPanorama = closestPanorama;

        //Draw the old fading panorama
        if (oldClosestPanorama != closestPanorama && oldClosestPanorama != NULL) {
            glColor4f(1, 1, 1, oldClosestOpacity);
            oldClosestPanorama->draw(referencePoint, true);
            glColor4f(1, 1, 1, 1);
            glClear(GL_DEPTH_BUFFER_BIT);

            //Fade
            oldClosestOpacity -= 0.05f;
            if (oldClosestOpacity <= 0) {
                oldClosestOpacity = 1;
                oldClosestPanorama = closestPanorama;
            }


        }



    }

    for (unsigned int i = 0; i < panoramas.size(); i++) {
        //glClear((GL_DEPTH_BUFFER_BIT));
        //if (panoramas[i] != closestPanorama)
        {
            //            if(i == panoramas.size()-1) {
            //                start = 80;
            //                end = 100;
            //            }

            panoramas[i]->draw(referencePoint);
        }
    }

    //Dots for debugging
    if (false && closestPanorama != NULL) {
        glPushMatrix();
        glTranslated(closestPanorama->location.easting - referencePoint.easting, closestPanorama->location.northing - referencePoint.northing, 0);
        glRotatef(180 - closestPanorama->pano_yaw_deg, 0, 0, 1);

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        glPointSize(2);
        glColor3f(1, 0, 0);
        glBegin(GL_POINTS);
        for (int y = 0; y < closestPanorama->mapHeight; y++) {
            for (int x = 0; x < closestPanorama->mapWidth; x++) {

                //                int index = closestPanorama->depthmapIndices[y * closestPanorama->mapWidth + x];
                int panoindex = closestPanorama->panomapIndices[y * closestPanorama->mapWidth + x];
                if (panoindex == closestPanorama->ownPanomapIndex)
                    glColor3f(1, 0, 0);
                else
                    glColor3f(0, 0, 1);

                //if(index == theDepthIndex)
                //closestPanorama->drawVertexAtAzimuthElevation(x, y);
                //glVertex3f(0, 0, 0);

            }

        }

        glEnd();
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glPopMatrix();
    }

    player.drawPlayer(referencePoint);

}

/**
 * Collision detection
 */
void Explorer::collision_detection() {
}