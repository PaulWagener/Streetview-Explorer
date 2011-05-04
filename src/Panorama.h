#ifndef PANORAMA_H
#define	PANORAMA_H

#include <vector>
#include "gl.h"
#include "Utm.h"

using namespace std;


#define PANOID_LENGTH 22
#define CACHE_FILENAME_LENGTH 20 + PANOID_LENGTH

struct depthMapPlane {
    float x, y, z;
    float d;
};

struct xy {
    int x, y;
};

struct link {
    float yaw_deg;
    char pano_id[PANOID_LENGTH+1];
};

struct renderSettings {
    bool transparancy;
    int verticalAccuracy;
    int horizontalDetail;
};

class Panorama {
public:
    bool isTransparant(int x, int y, int horizontal_step);
    bool isVisible(int x, int y);
    vector<unsigned char> uncompressed_image;
    void loadGL();
    int zoom_level;
    bool glLoaded;

    int compileList;
    int threeSixtyCompileList;
    
    struct renderSettings compiledRenderSettings;

    float opacity;
    
    int image_width, image_height;
    int tile_width, tile_height;
    char pano_id[PANOID_LENGTH+1];
    int num_zoom_levels;
    float lat, lng, original_lat, original_lng;
    float pano_yaw_deg, tilt_yaw_deg, tilt_pitch_deg;

    bool hasAdjacent(const char *pano_id);
    const char* getPanoIdInDirection(float direction);

    float getGroundHeight();

    //Depth map information
    int mapWidth, mapHeight;
    vector<unsigned char> depthmapIndices;
    vector<struct depthMapPlane> depthmapPlanes;

    //Panomap information
    vector<unsigned char> panomapIndices;
    char panoids[200][PANOID_LENGTH+1];
    int ownPanomapIndex;

    vector<struct link> links;

    static const char CACHEFILE_VERSION;

    //Texture information
    unsigned int texture_width, texture_height;
    GLuint texture_id;

    //Position in internal (UTM) coordinate system
    struct utmPosition location;

    float startFull;
    float endFull;
    int rank;
public:
    Panorama(const char* xml, int zoom_level);
    ~Panorama();

    static void getCacheFilename(const char* pano_id, int zoom_level, char filename[]);
    static bool isCached(const char *pano_id, int zoom_level);
    void draw(struct utmPosition referencePosition, bool drawAll=false);
    void drawActual(struct utmPosition referencePoint, bool drawAll, struct renderSettings settings);
    float distanceTo(struct utmPosition location);
    
public:
    void drawVertexAtAzimuthElevation(int x, int y, struct renderSettings settings);

    static void downloadAndCache(const char *pano_id, int zoom_level);
    void loadFromCache(const char* pano_id, int zoom_level);
    void loadXML(const char* url);
};

#endif	/* PANORAMA_H */

