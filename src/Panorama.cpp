#include "Panorama.h"
#include "string.h"
#include "stdio.h"
#include "base64.h"
#include <zlib.h>
#include <vector>
#include <math.h>
#include <cassert>
#include "download.h"
#include "Utm.h"
#include "common.h"
#include "Settings.h"
#include <string>
#include <locale.h>
#include "statustext.h"
#include <stdlib.h>
using namespace std;

const char Panorama::CACHEFILE_VERSION = 1;

/**
 * Will load the specified panorama either from disk or from Google
 * Depending on that it will take either a long time or a VERY long time.
 * Do not attempt to make a Panorama in the main thread!
 * 
 * @param pano_id
 * @param zoom_level
 */
Panorama::Panorama(const char* pano_id, int zoom_level)
: zoom_level(zoom_level),
glLoaded(false),
compileList(-1),
opacity(0) {
    if (strlen(pano_id) > PANOID_LENGTH)
        throw "pano_id longer than allowed number of characters";

    //Download cachefile if it doesn't exist yet
    if (!isCached(pano_id, zoom_level))
        Panorama::downloadAndCache(pano_id, zoom_level);

    try {
        loadFromCache(pano_id, zoom_level);
    } catch(const char* error) {
        char cachefile[CACHE_FILENAME_LENGTH+1];
        getCacheFilename(pano_id, zoom_level, cachefile);

        setStatus("Error loading cachefile '%s' (%s). Deleting it...", cachefile, error);
        unlink(cachefile);
        sleep(1);
        setStatus("");

        throw error;
    }
}

Panorama::~Panorama() {
    //Delete resources we allocated in OpenGL
    if (glLoaded) {
        glDeleteTextures(1, &texture_id);

        if (compileList != -1) {
            glDeleteLists(compileList, 1);
            glDeleteLists(threeSixtyCompileList, 1);
        }
    }
}

void Panorama::getCacheFilename(const char* pano_id, int zoom_level, char filename[]) {
    sprintf(filename, "cache/%d-%s.pano", zoom_level, pano_id);
}

/**
 * Check if a Panorama happens to be linked from this one via a road
 * @param pano_id
 * @return
 */
bool Panorama::hasAdjacent(const char *pano_id) {
    for (unsigned int i = 0; i < links.size(); i++) {
        if (strcmp(pano_id, links[i].pano_id) == 0)
            return true;
    }
    return false;
}

float Panorama::getGroundHeight() {
    int groundIndex = depthmapIndices[mapHeight * mapWidth - 1];
    return depthmapPlanes[groundIndex].d;
}

/**
 * Find out if a panorama was downloaded before
 * 
 * @param pano_id
 * @param zoom_level
 * @return
 */
bool Panorama::isCached(const char* pano_id, int zoom_level) {
    //The cachefile name
    char filename[CACHE_FILENAME_LENGTH+1];
    getCacheFilename(pano_id, zoom_level, filename);

    //Check if the cachefile exists
    if (FILE * file = fopen(filename, "rb")) {
        fclose(file);
        return true;
    }
    return false;
}

#include <math.h>

struct vec3 {
    float x, y, z;
};

/**
 * Check if a position on the map should be visible on screen.
 * A position is visible if it meets one of the following criteria:
 * - In the pano_map the position is indicated as to belong to this panorama
 *          (only if the pixel doesn't have infinite depth, like the sky)
 * - The pixel is within the [startFull, endFull] range
 *
 * @param x
 * @param y
 * @return
 */
bool Panorama::isVisible(int x, int y) {

    //Make sure the pixel is on the map
    if (x < 0) x += mapWidth;
    if (y < 0) y += mapHeight;
    if (x >= mapWidth) x -= mapWidth;
    if (y >= mapHeight) y -= mapHeight;

    int panoIndex = panomapIndices[y * mapWidth + x];
    int depthMapIndex = depthmapIndices[y * mapWidth + x];

    return panoIndex == ownPanomapIndex && depthMapIndex != 0;
}

/**
 * A vertex is transparant if in the 3x3 area
 * around the position there is a pixel that is not drawn
 * 
 * @param x
 * @param y
 * @return
 */
bool Panorama::isTransparant(int x, int y, int horizontal_step) {
    if (y > 0 && y < mapHeight - 1) {
        for (int _x = -horizontal_step; _x <= horizontal_step; _x += horizontal_step) {
            for (int _y = -1; _y <= 1; _y++) {
                if (!isVisible(x + _x, y + _y))
                    return true;

            }
        }
    }
    return false;
}

void Panorama::drawActual(struct utmPosition referencePoint, bool drawAll, struct renderSettings settings) {
    glPushMatrix();
    glTranslated(location.easting - referencePoint.easting, location.northing - referencePoint.northing, 0);
    glRotated(180 - pano_yaw_deg, 0, 0, 1);

    glBindTexture(GL_TEXTURE_2D, texture_id);
    //The environment is basically just a sphere with each vertex at a different distance creating
    //The illusion that everything is organized into planes

    /**
     * Draw the panorama in long vertical strips
     */
    for (int x = 0; x < mapWidth - 1; x += settings.horizontalDetail) {
        //printf("%d ", x);
        bool drawing = false;
        int currentDepthMap = 0;
        int currentDepthMap2 = 0;

        const int next_x = x + settings.horizontalDetail;
        int map_next_x = next_x;
        if (map_next_x >= mapWidth)
            map_next_x -= mapWidth;

        //Start drawing immediately at the zenith in the sky
        if (drawAll) {
            drawing = true;

            glBegin(GL_QUAD_STRIP);
            drawVertexAtAzimuthElevation(next_x, 0, settings);
            drawVertexAtAzimuthElevation(x, 0, settings);
        }
        const unsigned int endHeight = mapHeight - 1;

        for (unsigned int y = 0; y < endHeight; y += 1) {

            const int next_y = y + 1;

            //Stop drawing
            if (!drawAll && drawing && (!isVisible(x, y) || !isVisible(x + 1, y))) {

                drawing = false;

                drawVertexAtAzimuthElevation(next_x, y, settings);
                drawVertexAtAzimuthElevation(x, y, settings);
                glEnd();
            }

            /**
             * If the depth map changes ensure that extra vertices are placed to create a nice sharp
             * edge between planes.
             */
            const int depthMapIndex = depthmapIndices[(y + 1) * mapWidth + x];
            const int depthMapIndex2 = depthmapIndices[(y + 1) * mapWidth + map_next_x];

            if (drawing && (depthMapIndex != currentDepthMap || depthMapIndex2 != currentDepthMap2)) {
                currentDepthMap = depthMapIndex;
                currentDepthMap2 = depthMapIndex2;

                drawVertexAtAzimuthElevation(next_x, y, settings);
                drawVertexAtAzimuthElevation(x, y, settings);

                drawVertexAtAzimuthElevation(next_x, next_y, settings);
                drawVertexAtAzimuthElevation(x, next_y, settings);

                //Draw extra perspective corrective lines
            } else if (drawing && (y % settings.verticalAccuracy) == 0) {

                drawVertexAtAzimuthElevation(next_x, y, settings);
                drawVertexAtAzimuthElevation(x, y, settings);

                //Draw extra lines for transparancy to flow nicely
            } else if (!drawAll && drawing && (isTransparant(x, y - 1, settings.horizontalDetail) || isTransparant(next_x, y - 1, settings.horizontalDetail))) {
                drawVertexAtAzimuthElevation(next_x, y, settings);
                drawVertexAtAzimuthElevation(x, y, settings);
            }//Start drawing
            else if (!drawing && isVisible(x, y) && isVisible(x + 1, y)) {
                drawing = true;

                glBegin(GL_QUAD_STRIP);

                currentDepthMap = depthMapIndex;
                currentDepthMap2 = depthMapIndex2;

                drawVertexAtAzimuthElevation(next_x, y, settings);
                drawVertexAtAzimuthElevation(x, y, settings);
            }

            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


        }
        //Draw last vertex at the nadir / very bottom of panorama
        if (drawing) {
            drawVertexAtAzimuthElevation(next_x, endHeight, settings);
            drawVertexAtAzimuthElevation(x, endHeight, settings);

            glEnd();
        }

    }

    glPopMatrix();
}

/**
 * Draws the panorama, the real drawing is in the other draw function. This just tries to create a display
 * list out of it as aggresively as it can. If parameters or settings change we are forced to generate a new
 * list.
 *
 * @param referencePoint
 * @param startFull
 * @param endFull
 */
void Panorama::draw(struct utmPosition referencePoint, bool drawAll) {
    /**
     * Transparant borders are the fuzzy edges of the panorama. Due to the use of display
     * lists it is impossible to both have a continually increasing opacity AND transparant borders.
     *
     * We solve this in a two step process: during the fade-in sequence we set transparancy globally,
     * no glColor calls are allowed in the display list that could corrupt the global transparancy.
     * Once faded in we enable the transparant borders and allow glColor calls in the display list
     */
    bool settingsChanged = false;
    bool firstTime = false;

    if (compileList == -1) {
        compileList = glGenLists(1);
        threeSixtyCompileList = glGenLists(1);
        firstTime = true;
    }

    //If any setting changed from the last time we compiled the list we need to recompile it with the new settings
    if (compiledRenderSettings.horizontalDetail != settings.horizontal_accuracy
            || compiledRenderSettings.verticalAccuracy != settings.vertical_accuracy) {
        settingsChanged = true;
    }

    //Recreate the display lists
    if (firstTime || settingsChanged) {
        struct renderSettings renderSettings;
        renderSettings.horizontalDetail = settings.horizontal_accuracy;
        renderSettings.verticalAccuracy = settings.vertical_accuracy;

        //Recreate display list for 'only own data' panorama
        renderSettings.transparancy = true;
        glNewList(compileList, GL_COMPILE);
        drawActual(referencePoint, false, renderSettings);
        glEndList();

        //Recreate display list for 'all data' panorama
        renderSettings.transparancy = false;
        glNewList(threeSixtyCompileList, GL_COMPILE);
        drawActual(referencePoint, true, renderSettings);
        glEndList();

        compiledRenderSettings = renderSettings;
    }

    //Draw the cached version
    if (drawAll) {
        glCallList(threeSixtyCompileList);
    } else {
        glCallList(compileList);
    }





}

/**
 * Draw a single vertex at a specific coordinate on the sphere.
 */
void Panorama::drawVertexAtAzimuthElevation(int x, int y, struct renderSettings settings) {
    //Set vertex transparancy
    if (settings.transparancy)
        glColor4d(1, 1, 1, isTransparant(x, y, settings.horizontalDetail) ? 0 : 1);


    float rad_azimuth = x / (float) (mapWidth - 1.0f) * TWICE_PI;
    float rad_elevation = y / (float) (mapHeight - 1.0f) * PI;

    //Calculate the cartesian position of this vertex (if it was at unit distance)
    vec3 xyz;
    xyz.x = sin(rad_elevation) * sin(rad_azimuth);
    xyz.y = sin(rad_elevation) * cos(rad_azimuth);
    xyz.z = cos(rad_elevation);
    float distance = 1;

    //Value that is safe to use to retrieve stuff from the index arrays
    const int map_x = x % mapWidth;

    //Calculate distance of point according to the depth map data.
    int depthMapIndex = depthmapIndices[y * mapWidth + map_x];
    if (depthMapIndex == 0) {
        //Distance of sky
        distance = 100;

    } else {
        struct depthMapPlane plane = depthmapPlanes[depthMapIndex];
        distance = -plane.d / (plane.x * xyz.x + plane.y * xyz.y + -plane.z * xyz.z);
    }

    //Draw point!
    glTexCoord2d(x / (float) mapWidth, y / (float) mapHeight);
    glVertex3f(xyz.x * distance, xyz.y*distance, xyz.z * distance);
}

/**
 * Loads the panorama from a file as was downloaded from downloadAndCache()
 * This method assumes an OpenGL context to be present!
 * 
 * @param pano_id
 * @param zoom_level
 */
void Panorama::loadFromCache(const char *pano_id, int zoom_level) {
    char cachefile[CACHE_FILENAME_LENGTH + 1];
    getCacheFilename(pano_id, zoom_level, cachefile);

    FILE *f = fopen(cachefile, "rb");
    if (!f)
        throw "Could not open cachefile";

    char version;
    int imageOffset;
    fread(&version, sizeof (version), 1, f);
    if (version != CACHEFILE_VERSION)
        throw "Unexpected cachefile version...";

    fread(&imageOffset, sizeof (imageOffset), 1, f);

    //Read in XML data
    {
        int xmlSize;
        fread(&xmlSize, sizeof (xmlSize), 1, f);

        std::vector<char> xmlData(xmlSize);
        fread(&xmlData[0], xmlSize, 1, f);
        loadXML((const char*) &xmlData[0]);
    }

    //Read in image
    {
        fseek(f, imageOffset, SEEK_SET);

        //Image data
        long compressedImageSize;
        unsigned int width, height;
        fread(&width, sizeof (width), 1, f);
        fread(&height, sizeof (height), 1, f);
        fread(&compressedImageSize, sizeof (compressedImageSize), 1, f);

        //Read in image
        vector<unsigned char> compressed_image(compressedImageSize);
        fread(&compressed_image[0], compressedImageSize, 1, f);

        //That was the last thing to be read from the file
        fclose(f);

        //Uncompress image
        unsigned long size = width * height * 3;
        uncompressed_image.resize(size);
        texture_width = width;
        texture_height = height;
        int code = uncompress(&uncompressed_image[0], &size, &compressed_image[0], compressedImageSize);

        if (code != Z_OK)
            throw "Unable to uncompress image texture";

        if (size != width * height * 3)
            throw "Unexpected size of uncompressed image";
    }
    /**
     * File format:
     * char     version
     * int      imageOffset
     * int size of xml contents
     * xml contents of Google panorama description (not \0 terminated)
     *
     * at imageOffset:
     *   int    width
     *   int    height
     *   long    size   size of data to follow
     *   RGB raw image ( compressed with standard zlib compress() )
     */
}

/**
 * Load all information from the contents of a url like
 * http://maps.google.com/cbk?output=xml&panoid=qdAqxSu085_gynN0R8k4RA&dm=1&pm=1
 * 
 * @param xml
 */
void Panorama::loadXML(const char *xml) {
    //Not sure who the culprit is, but somewhere out there someone changes the locale away from the
    //portable "C" locale, I'm suspecting wxWidgets is to blame.
    //This has the side effect that floats don't parse anymore because sscanf() expects
    //they have comma's instead of dots as the decimal operator
    setlocale(LC_ALL, "C");

    //Scan the data properties
    int found = sscanf(strstr(xml, "<data_properties"),
            "<data_properties image_width=\"%d\" image_height=\"%d\" tile_width=\"%d\" tile_height=\"%d\" pano_id=\"%[_-A-Za-z0-9]\" num_zoom_levels=\"%d\" lat=\"%f\" lng=\"%f\" original_lat=\"%f\" original_lng=\"%f\">",
            &image_width, &image_height, &tile_width, &tile_height, (char*) &pano_id, &num_zoom_levels, &lat, &lng, &original_lat, &original_lng);

    if (found != 10)
        throw "Unexpected <data_properties> data";

    //Scan the projection properties
    found = sscanf(strstr(xml, "<projection_properties"),
            "<projection_properties projection_type=\"spherical\" pano_yaw_deg=\"%f\" tilt_yaw_deg=\"%f\" tilt_pitch_deg=\"%f\"",
            &pano_yaw_deg, &tilt_yaw_deg, &tilt_pitch_deg);

    if (found != 3)
        throw "Unexpected <projection_properties> data";

    //Read in the <link> data (just the yaw and id part)
    struct link link;
    const char *linkString = xml;
    while ((linkString = strstr(linkString + 1, "<link ")) != NULL) {
        sscanf(linkString, "<link yaw_deg=\"%f\" pano_id=\"%[_-A-Za-z0-9]\"", &link.yaw_deg, (char*) &link.pano_id);
        links.push_back(link);
    }

    //Decode the depth map
    //The depth map is encoded as a series of pixels in a 512x256 image. Each pixels refers
    //to a depthMapPlane which are also encoded in the data. Each depthMapPlane has three elements:
    //The x,y,z normals and the closest distance the plane has to the origin. This uniquely
    //identifies the plane in 3d space.
    {
        //Get the base64 encoded data
        string depth_map_base64;
        {
            const char* begin = strstr(xml, "<depth_map>");
            const char* end = strstr(xml, "</depth_map>");
            if (begin == NULL || end == NULL)
                throw "No depth map information found in xml data";

            depth_map_base64 = std::string(begin + strlen("<depth_map>"), end);
        }

        //Decode base64
        vector<unsigned char> depth_map_compressed(depth_map_base64.length());
        int compressed_length = decode_base64(&depth_map_compressed[0], &depth_map_base64[0]);

        //Uncompress data with zlib
        //TODO: decompress in a loop so we can accept any size 
        unsigned long length = 512 * 256 + 5000;
        vector<unsigned char> depth_map(length);
        int zlib_return = uncompress(&depth_map[0], &length, &depth_map_compressed[0], compressed_length);
        if (zlib_return != Z_OK)
            throw "zlib decompression of the depth map failed";

        //Load standard data
        const int headersize = depth_map[0];
        const int numPanos = depth_map[1] | (depth_map[2] << 8);
        mapWidth = depth_map[3] | (depth_map[4] << 8);
        mapHeight = depth_map[5] | (depth_map[6] << 8);
        const int panoIndicesOffset = depth_map[7];

        if (headersize != 8 || panoIndicesOffset != 8)
            throw "Unexpected depth map header";

        //Load depthMapIndices
        depthmapIndices = vector<unsigned char>(mapHeight * mapWidth);
        memcpy(&depthmapIndices[0], &depth_map[panoIndicesOffset], mapHeight * mapWidth);

        //Load depthMapPlanes
        depthmapPlanes = vector<struct depthMapPlane > (numPanos);
        memcpy(&depthmapPlanes[0], &depth_map[panoIndicesOffset + mapHeight * mapWidth], numPanos * sizeof (struct depthMapPlane));
    }


    LatLonToUtmWGS84(this->location, this->original_lat, this->original_lng);

    //Decode the pano map
    {
        //Get the base64 encoded data
        std::string pano_map_base64;
        {
            const char* begin = strstr(xml, "<pano_map>");
            const char* end = strstr(xml, "</pano_map>");
            if (begin == NULL || end == NULL)
                throw "No pano map information found in xml data";

            pano_map_base64 = std::string(begin + strlen("<pano_map>"), end);
        }

        //Decode base64
        vector<unsigned char> pano_map_compressed(pano_map_base64.length());
        int compressed_length = decode_base64(&pano_map_compressed[0], pano_map_base64.c_str());

        //Uncompress data with zlib
        unsigned long length = mapWidth * mapHeight + 5000;
        vector<unsigned char> pano_map(length);
        int zlib_return = uncompress(&pano_map[0], &length, &pano_map_compressed[0], compressed_length);
        if (zlib_return != Z_OK)
            throw "zlib decompression of the pano map failed";

        //Load standard data
        const int headersize = pano_map[0];
        const int numPanos = pano_map[1] | (pano_map[2] << 8);
        const int panomapWidth = pano_map[3] | (pano_map[4] << 8);
        const int panomapHeight = pano_map[5] | (pano_map[6] << 8);
        const int panoIndicesOffset = pano_map[7];

        if (headersize != 8 || panoIndicesOffset != 8
                || panomapWidth != mapWidth || panomapHeight != mapHeight)
            throw "Unexpected pano map header";

        //Load panomap indices
        panomapIndices = vector<unsigned char>(mapWidth * mapHeight);
        memcpy(&panomapIndices[0], &pano_map[panoIndicesOffset], mapWidth * mapHeight);

        //Load pano-ids
        //Remember: the panoids array is 0-indexed,
        //while the pano id's in panomapIndices are 1-indexed (0 being reserved for sky)
        //Don't forget to subtract 1 from the index when accessing panoids!
        const int panoid_offset = panoIndicesOffset + mapWidth*mapHeight;
//        printf("%d\n", numPanos);
//        exit(0);
        memset(&panoids, '\0', (PANOID_LENGTH + 1) * numPanos); //Makes sure all strings are properly \0 terminated
        for (int i = 0; i < numPanos; i++) {
            memcpy(&panoids[i], &pano_map[panoid_offset + PANOID_LENGTH * i], PANOID_LENGTH);

            //All panomap indices have some pixels that refer to themselves,
            //Usually the index is 1 or 2 (0 is reserved for sky and other infinite depth spots)
            if (memcmp(&panoids[i], &pano_id, PANOID_LENGTH + 1) == 0)
                ownPanomapIndex = i + 1;
        }
    }

}

/**
 * Download all datafiles that are needed for this panorama and cache them in a file half-processed
 * @return
 */
void Panorama::downloadAndCache(const char* pano_id, int zoom_level) {
    const unsigned int TILE_HEIGHT = 512;
    const unsigned int TILE_WIDTH = 512;

    //If you ask for an image of zoom level 0 you get an image of this dimension.
    //(actually bigger but the image wraps horizontally and is padded with black vertically)
    const int ZOOM0_WIDTH = 416;
    const int ZOOM0_HEIGHT = 208;

    //Dimensions of the image at a particular zoom_level
    const int width = ZOOM0_WIDTH * 1 << zoom_level;
    const int height = ZOOM0_HEIGHT * 1 << zoom_level;

    //Create texture
    vector<unsigned char> pano_image(width * height * 3);

    //Download each individual tile and compose them into the big texture
    for (int tile_y = 0; tile_y < ceil(height / (float) TILE_HEIGHT); tile_y++) {
        for (int tile_x = 0; tile_x < ceil(width / (float) TILE_WIDTH); tile_x++) {

            //Download tile in RGB format
            char url[150];
            sprintf((char*) &url, "http://cbk0.google.com/cbk?output=tile&panoid=%s&zoom=%d&x=%d&y=%d", pano_id, zoom_level, tile_x, tile_y);
            struct image_block tile = download_jpeg(url);

            if (tile.width != TILE_WIDTH ||
                    tile.height != TILE_HEIGHT) {
                throw "Downloaded tile had unexpected dimensions";
            }

            //Copy each pixel from the tile to the global texture
            for (unsigned int y = 0; y < tile.height; y++) {
                for (unsigned int x = 0; x < tile.width; x++) {

                    const int global_x = tile_x * tile.width + x;
                    const int global_y = tile_y * tile.height + y;

                    if (global_x >= width || global_y >= height)
                        continue;

                    pano_image[(global_y * width + global_x)*3 + 0] = tile.data[(y * tile.width + x)*3 + 0];
                    pano_image[(global_y * width + global_x)*3 + 1] = tile.data[(y * tile.width + x)*3 + 1];
                    pano_image[(global_y * width + global_x)*3 + 2] = tile.data[(y * tile.width + x)*3 + 2];
                }
            }

            free(tile.data);
        }
    }

    //Cache the panorama to a file
    {
        //Download xml data
        char xml_url[100];
        sprintf(xml_url, "http://maps.google.com/cbk?output=xml&panoid=%s&dm=1&pm=1", pano_id);
        const std::auto_ptr<std::vector<unsigned char> > xmlData = download(xml_url);

        //Open cache file
        char cachefile[CACHE_FILENAME_LENGTH + 1];
        getCacheFilename(pano_id, zoom_level, cachefile);

        FILE *f = fopen(cachefile, "wb");
        if (!f)
            throw "Unable to open cache file";

        //Write version
        fwrite(&CACHEFILE_VERSION, sizeof (CACHEFILE_VERSION), 1, f);

        //Write offset
        const int imageOffset = sizeof (char) + 2 * sizeof (int) +xmlData->size(); //Image begins after a char and 2 int plus the size of the xmlData
        fwrite(&imageOffset, sizeof (imageOffset), 1, f);

        //Write xml data
        {
            assert(xmlData->size() < (1ULL << 32));
            const int size = xmlData->size();
            fwrite(&size, sizeof (size), 1, f);
            fwrite(&(*xmlData)[0], 1, xmlData->size(), f);
        }

        //Write the image metadata
        fwrite(&width, sizeof (width), 1, f);
        fwrite(&height, sizeof (height), 1, f);

        //Compress the image and write it to the file
        vector<unsigned char> compressed_pano;
        compressed_pano.reserve(compressBound(height * width * 3));

        unsigned long size = height * width * 3;
        compress(&compressed_pano[0], &size, (const Bytef*) &pano_image[0], size);

        fwrite(&size, sizeof (size), 1, f);
        fwrite(&compressed_pano[0], size, 1, f);

        //Close stuff that is open
        fclose(f);
    }

}

/**
 * Find out how far a position is from this panorama

 * @return distance in meters
 */
float Panorama::distanceTo(struct utmPosition location) {
    float horizontalDist = (float) fabs(location.easting - this->location.easting);
    float verticalDist = (float) fabs(location.northing - this->location.northing);

    return sqrt(horizontalDist * horizontalDist + verticalDist * verticalDist);
}

/**
 * Function to be called when everything is loaded up in memory ready to go into OpenGL.
 * It is designed to do as little processing / loading as possible so it is safe to call in the
 * main draw loop. It assumes that the texture has already been put into the uncompressed_image field.
 */
void Panorama::loadGL() {
    //Create texture
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    //Choose between mipmapping (better quality) and linear interpolation (faster)
    if (settings.mipmapping) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3,
                texture_width, texture_height,
                GL_RGB, GL_UNSIGNED_BYTE, &uncompressed_image[0]);

    } else {
        const int level = 0; //Mipmap level, must be zero if we want to display anything in GL_LINEAR mode
        const int border = 0; //????
        const int channels = 3; //Only load the 3 primary color: red, green, blue
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, level, channels, texture_width, texture_height, border, GL_RGB, GL_UNSIGNED_BYTE, &uncompressed_image[0]);
    }
    uncompressed_image.clear();

    glLoaded = true;
}