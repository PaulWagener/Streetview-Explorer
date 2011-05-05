#include "download.h"
#include "statustext.h"

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <jpeglib.h>
#include <jerror.h>
#include <curl/curl.h>

#define USER_AGENT "StreetView Explorer 1.0"

size_t write_download_data(void *ptr, size_t size, size_t nmemb, std::vector<unsigned char>* b) {
    size_t oldSize = b->size();
    try {
        b->resize(b->size() + size * nmemb);
    } catch (const std::bad_alloc&) {
        throw "download failed: out of memory";
    }
    memcpy(&(*b)[oldSize], ptr, size * nmemb);
    return size * nmemb;
}

/**
 * Download the contents of a url as a block of memory
 *
 * @param url
 * @return
 */
std::auto_ptr<std::vector<unsigned char> > download(const char *url) {
    setStatus("Downloading %s\n", url);

    std::auto_ptr<std::vector<unsigned char> > b(new std::vector<unsigned char>());

	
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_download_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, b.get());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);


    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);


    //Throw a human readable string back to the caller if something went wrong
    if (res != 0)
        throw curl_easy_strerror(res);

    return b;
}


/**
 * Downloads a jpeg from a url and returns it as raw RGB data in memory
 * @param url
 * @return
 */
struct image_block download_jpeg(const char *url) {
    const std::auto_ptr<std::vector<unsigned char> > jpeg_data = download(url);
    struct image_block image;

    //Initialize jpeg decompression
    struct jpeg_decompress_struct info;
    struct jpeg_error_mgr err;
    info.err = jpeg_std_error(&err);
    jpeg_create_decompress(&info);

    //Process jpeg
    jpeg_mem_src(&info, &(*jpeg_data)[0], jpeg_data->size());
    jpeg_read_header(&info, TRUE);
    jpeg_start_decompress(&info);

    //Read in basic image information
    if (info.num_components != 3)
        throw ".jpg file has unsupported amount of channels";

    image.width = info.output_width;
    image.height = info.output_height;
    image.data = (unsigned char*) malloc(3 * image.width * image.height);

    if (image.data == NULL)
        throw "Unable to allocate memory for storing raw .jpg file";

    //Parse image data per row
    for (unsigned int y = 0; y < image.height; y++) {
        unsigned char *scanline_pointer = &image.data[y * image.width*3];
        jpeg_read_scanlines(&info, &scanline_pointer, 1);
    }

    //Clean up
    jpeg_finish_decompress(&info);

    if(info.err->num_warnings > 0)
        throw "Corrupt jpeg downloaded";

    return image;
}
