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

#if __WXGTK__
/* Read JPEG image from a memory segment */
static void init_source (j_decompress_ptr cinfo) {}
static boolean fill_input_buffer (j_decompress_ptr cinfo)
{
    ERREXIT(cinfo, JERR_INPUT_EMPTY);
return TRUE;
}
static void skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
    struct jpeg_source_mgr* src = (struct jpeg_source_mgr*) cinfo->src;

    if (num_bytes > 0) {
        src->next_input_byte += (size_t) num_bytes;
        src->bytes_in_buffer -= (size_t) num_bytes;
    }
}
static void term_source (j_decompress_ptr cinfo) {}
static void jpeg_mem_src (j_decompress_ptr cinfo, void* buffer, long nbytes)
{
    struct jpeg_source_mgr* src;

    if (cinfo->src == NULL) {   /* first time for this JPEG object? */
        cinfo->src = (struct jpeg_source_mgr *)
            (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
            sizeof(struct jpeg_source_mgr));
    }

    src = (struct jpeg_source_mgr*) cinfo->src;
    src->init_source = init_source;
    src->fill_input_buffer = fill_input_buffer;
    src->skip_input_data = skip_input_data;
    src->resync_to_restart = jpeg_resync_to_restart; /* use default method */
    src->term_source = term_source;
    src->bytes_in_buffer = nbytes;
    src->next_input_byte = (JOCTET*)buffer;
}
#endif


/**
 * Downloads a jpeg from a url and returns it as raw RGB data in memory
 * @param url
 * @return
 */
struct image_block download_jpeg(const char *url) {
    const std::auto_ptr<std::vector<unsigned char> > jpeg_data = download(url);
    struct image_block image;

    //Prevent feeding invalid data to the jpeg decompression algorithm because it will just abruptly exit()
    if((*jpeg_data)[0] != 0xFF || ((*jpeg_data)[1] != 0xD8 && (*jpeg_data)[2] != 0xFF))
        throw "File not valid JPEG";

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
