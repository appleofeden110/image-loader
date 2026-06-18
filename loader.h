#pragma once 

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include "zlib.h"

typedef enum {
    FORMAT_JPEG,
    FORMAT_PNG,
    FORMAT_UNKNOWN
} ImageFormat;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint8_t channels;
    ImageFormat format;
    uint8_t bit_depth;
    uint8_t interlace;
} ImageInfo;

typedef struct {
    uint32_t       len;
    unsigned char  type[4];
    unsigned char* data;
    uint32_t       crc;
} ImageChunk;


uint32_t read_u32_be(unsigned char* buf);
uint8_t paeth(uint8_t a, uint8_t b, uint8_t c);

ImageFormat detectFormat(unsigned char* buf, long size);
ImageInfo get_png_info(unsigned char* buf, long size);
ImageChunk get_png_data(unsigned char* buf, uint32_t cursor);

uint8_t *process_png(const char *path, int *width, int *height, int *nrChannels);

