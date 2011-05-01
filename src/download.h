/* 
 * File:   download.h
 * Author: paulwagener
 *
 * Created on 3 april 2011, 12:43
 */

#ifndef DOWNLOAD_H
#define	DOWNLOAD_H

#include <sys/types.h>
#include <memory>
#include <vector>

struct block {
    unsigned char *data;
    int size;
};

struct image_block {
    unsigned int width;
    unsigned int height;
    unsigned char *data;
};

struct image_block download_jpeg(const char *url);
std::auto_ptr<std::vector<unsigned char> > download(const char *url);

#endif	/* DOWNLOAD_H */

