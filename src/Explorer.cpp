#include "Explorer.h"
#include "gl.h"
#include "stdio.h"
#include "common.h"
#include "Panorama.h"
#include "Settings.h"
#include <math.h>
#include "statustext.h"
#include <cstring>

Explorer::Explorer(const char* firstPano) {
    requestDownloadThread = false;
    downloading = false;
    downloadedPano = NULL;

    oldClosestPanorama = NULL;
    oldClosestOpacity = 1;

    currentMipmapping = settings.mipmapping;
    currentZoomlevel = settings.zoom_level;

    glInitialized = false;

    //Temporarily store the first panorama we should load
    //until we have an OpenGL context to actually load them in
    strncpy((char*) &firstPanorama, firstPano, PANOID_LENGTH + 1);
}

Explorer::~Explorer() {
    for (unsigned int i = 0; i < panoramas.size(); i++) {
        delete panoramas[i];
    }
    panoramas.clear();
}

/**
 * Loads a new panorama in a different thread
 * 
 * @param panoid
 * @param zoom_level
 */
void Explorer::loadPanorama(const char *panoid, int zoom_level) {
    if (downloading)
        return;

    //File is not cached, request a download procedure
    strncpy(downloadPano, panoid, PANOID_LENGTH + 1);
    downloadZoomLevel = zoom_level;
    downloading = true;
    requestDownloadThread = true;
}

/**
 * Check if a panorama with a certain ID is already present in the panoramas vector
 *
 * @param pano_id
 * @param zoom_level
 * @return
 */
bool Explorer::gotPanorama(const char* pano_id, int zoom_level) {
    for (unsigned int i = 0; i < panoramas.size(); i++) {
        if (strcmp(panoramas[i]->pano_id, pano_id) == 0 && panoramas[i]->zoom_level == zoom_level)
            return true;
    }
    return false;
}

bool Explorer::gotPanorama(Panorama *panorama) {
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

/**
 * 
 */
void Explorer::updatePanoramas() {
    if (downloading)
        return;

    //Delete all active panorama's if the textures need to be reloaded due to changing settings

    if (settings.zoom_level != currentZoomlevel || settings.mipmapping != currentMipmapping) {

        //Make sure we initialize back at the panorama we were last standing
        Panorama* closestPanorama = getClosestPanorama();
        if(closestPanorama != NULL)
            strcpy(firstPanorama, closestPanorama->pano_id);

        for (unsigned int i = 0; i < panoramas.size(); i++) {
            delete panoramas[i];
        }
        panoramas.clear();
        currentZoomlevel = settings.zoom_level;
        currentMipmapping = settings.mipmapping;


    }

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

    //Allow player to jump to other panorama's without there being an explicit link between them.
    //Looks up where in the panomap the player stands and looks up to which panorama that points belong
    //If it isn't any of the adjacent ones load it up, with any luck it doesn't get deleted immediately because
    //it is the new closest panorama
    if (closestPanorama != NULL) {

        //Get player location in texture coordinates of closest texture

        //Vector to the player from the closest panorama origin
        const float pv_x = (float) (player.location.northing - closestPanorama->location.northing);
        const float pv_y = (float) (player.location.easting - closestPanorama->location.easting);
        const float pv_z = player.height;

        float elevation = pv_z / sqrt(pv_x * pv_x + pv_y * pv_y + pv_z * pv_z);
        float azimuth = atan2(pv_y, pv_x);
        if (azimuth < 0)
            azimuth += TWICE_PI;
        azimuth = azimuth / TWICE_PI * 360;
        azimuth += (180 - closestPanorama->pano_yaw_deg);
        while (azimuth > 360) azimuth -= 360;
        while (azimuth < 0) azimuth += 360;

        const int x = (int) (azimuth / (float) 360 * closestPanorama->mapWidth) % closestPanorama->mapWidth;
        const int y = (int) (closestPanorama->mapHeight / 2 + -elevation * closestPanorama->mapHeight / 2) % closestPanorama->mapHeight;

        const int pano_index = closestPanorama->panomapIndices[y * closestPanorama->mapWidth + x];
        const char* pano_id = closestPanorama->panoids[pano_index - 1];

        if (pano_index != closestPanorama->ownPanomapIndex
                && pano_index != 0
                && strlen(pano_id) == PANOID_LENGTH
                && !closestPanorama->hasAdjacent(pano_id)) {
            loadPanorama(pano_id, settings.zoom_level);
        }
    }


    //If no panorama's have yet been downloaded load the panorama that was given in the constructor
    if (panoramas.size() == 0) {
        loadPanorama(firstPanorama, settings.zoom_level);
    } else if (closestPanorama != NULL) {

        //First delete all panorama's that are not adjacent to the closest panorama
        //and are not the closestPanorama itself
        for (unsigned int i = 0; i < panoramas.size(); i++) {
            if (panoramas[i] != closestPanorama && !closestPanorama->hasAdjacent(panoramas[i]->pano_id)) {
                delete panoramas[i];
                panoramas.erase(panoramas.begin() + i);
                i--;
            }
        }

        //Then see if there are any adjacent panorama's not already loaded
        for (unsigned int i = 0; i < closestPanorama->links.size(); i++) {
            if (!gotPanorama(closestPanorama->links[i].pano_id, settings.zoom_level))
                loadPanorama(closestPanorama->links[i].pano_id, settings.zoom_level);
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

void Explorer::display(int width, int height) {

    if (!glInitialized) {

#ifdef __GLEWINIT__
        glewInit();
#endif

        glShadeModel(GL_SMOOTH);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        //Light is only used to illuminate Streetview Guy,
        //The panorama's themselves are drawn at full intensity without lighting.
        glEnable(GL_LIGHT0);

        GLfloat ambient[] = {0.8f, 0.8f, 0.8f, 1.0f};
        GLfloat diffuse[] = {0.6f, 0.7f, 0.7f, 1.0f};
        GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
        GLfloat position[] = {0.0f, 0.0f, 0.0f, 1.0f};

        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
        glLightfv(GL_LIGHT0, GL_POSITION, position);


        glEnable(GL_TEXTURE_2D);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);

        const int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        const char *p = "uniform sampler2D tex;uniform float alpha; void main(){gl_FragColor = gl_Color * texture2D(tex, gl_TexCoord[0].xy);gl_FragColor.a = gl_Color.w*alpha;}";
        glShaderSource(fragment_shader, 1, &p, NULL);
        glCompileShader(fragment_shader);

        program = glCreateProgram();
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);
        glUseProgram(program);

        glInitialized = true;
    }

    updatePanoramas();
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, width / (GLdouble) height, 1, 300.0);

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

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    player.updatePosition();
    glLoadIdentity();
    player.targetCamera(referencePoint);

    if (settings.wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    Panorama *closestPanorama = getClosestPanorama();

    //Ensure oldClosestPanorama ain't a dangling pointer
    if (!gotPanorama(oldClosestPanorama))
        oldClosestPanorama = NULL;

    //Increase opacity of all Panorama's
    for (unsigned int i = 0; i < panoramas.size(); i++) {
        panoramas[i]->opacity += 0.03f;
        if (panoramas[i]->opacity > 1)
            panoramas[i]->opacity = 1;
    }

    glColor3f(1, 1, 1);

    glUseProgram(program);
    const int alphaUniform = glGetUniformLocation(program, "alpha");
    glEnable(GL_TEXTURE_2D);

    /**
     * Draw the background, this is from the panorama the closest
     * to the camera because it has the best perspective
     */
    if (closestPanorama != NULL) {
        player.target_height = -closestPanorama->getGroundHeight();

        glUniform1f(alphaUniform, 1);
        closestPanorama->draw(referencePoint, true);
        glClear(GL_DEPTH_BUFFER_BIT);

        if (oldClosestPanorama == NULL)
            oldClosestPanorama = closestPanorama;

        //If another panorama is fading away draw that over the current one with a faded opacity
        if (oldClosestPanorama != closestPanorama && oldClosestPanorama != NULL) {

            glUniform1f(alphaUniform, oldClosestOpacity);
            oldClosestPanorama->draw(referencePoint, true);
            glClear(GL_DEPTH_BUFFER_BIT);

            //Fade
            oldClosestOpacity -= 0.05f;
            if (oldClosestOpacity <= 0) {
                oldClosestOpacity = 1;
                oldClosestPanorama = closestPanorama;
            }
        }

    }

    //Draw all other panorama's (including the closest just to fill the depth buffer)
    for (unsigned int i = 0; i < panoramas.size(); i++) {
        glUniform1f(alphaUniform, panoramas[i]->opacity);
        panoramas[i]->draw(referencePoint);
    }

    glUseProgram(0);

    //Draw the player
    player.drawPlayer(referencePoint);
}

/**
 * Starts where loadPanorama() leaves off. (Down)load a panorama to the cache
 * and in memory. The panorama still needs a loadGL() call in the OpenGL thread to load in textures
 * before it can be displayed.
 */
void Explorer::downloadThread() {
    try {
        Panorama *p = new Panorama(downloadPano, downloadZoomLevel);
        downloadedPano = p;
        setStatus(""); //Reset the status text, we loaded the panorama succesfully
    } catch (const char* c) {
        sleep(1); //Allow to see what the last message was
        setStatus("Error: %s", c);
        sleep(2); //Allow to see the error
    }
    downloading = false;
}
