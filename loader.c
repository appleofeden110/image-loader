#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "zlib.h"


#define EXIF_METADATA_SIZE 65535 // 64 kb
#define IHDR_END 29


// PNG chunks
// chunks IHDR -> [PLTE (pallete); IDAT (Image Data); ancillary (tEXt, gAMA, sRGB)] -> IEND (0-byte end marker)
// chunk anatomy: Len (4 bytes) -> Type (4 ASCII chars OR 4 bytes) -> Data (Custom Length specified by the Length chunk) ->  CRC32 (4 bytes)
static const unsigned char PNG_SOI[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
static const unsigned char IHDR[] = {0x49, 0x48, 0x44, 0x52}; // main chunk
static const unsigned char PLTE[] = {0x50, 0x4C, 0x54, 0x45}; // pallete, rarely seen
static const unsigned char IEND[] = {0x49, 0x45, 0x4E, 0x44}; // end 0-byte info chunk
static const unsigned char IDAT[] = {0x49, 0x44, 0x41, 0x54}; // image data chunk

// JPEG chunks
static const unsigned char JPEG_MARKER = 0xFF; // marker for start of every upcoming chunk
static const unsigned char SOI = 0xD8; // start of image
static const unsigned char SOF = 0xC0; // start of frame 0
static const unsigned char DHT = 0xC4; // define huffman table
static const unsigned char DQT = 0xDB; // define quantization table 
static const unsigned char SOS = 0xDA; // start of scan
static const unsigned char EOI = 0xD9; // end of image


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

uint32_t read_u32_be(unsigned char* buf) {
	return ((uint32_t)buf[0] << 24) | 
	       ((uint32_t)buf[1] << 16) | 
	       ((uint32_t)buf[2] << 8) |
	       ((uint32_t)buf[3]);
}


ImageInfo get_png_info(unsigned char* buf, long size) {
    ImageInfo info = {0};
    info.format = FORMAT_PNG;
    info.width      = read_u32_be(buf + 16);
    info.height     = read_u32_be(buf + 20);
    info.bit_depth  = buf[24];
    uint8_t color_type = buf[25];
    info.interlace = buf[28];

    switch (color_type) {
        case 0: info.channels = 1; break; // grayscale
        case 2: info.channels = 3; break; // RGB
        case 3: info.channels = 1; break; // indexed
        case 4: info.channels = 2; break; // grayscale + alpha
        case 6: info.channels = 4; break; // RGBA
    }

    return info;
}



ImageChunk get_png_data(unsigned char* buf, uint32_t cursor) {
    ImageChunk chunk = {0};

    chunk.len = read_u32_be(buf+cursor-4);
    memcpy(chunk.type, buf+cursor, 4);
    chunk.data = buf+cursor+4;
    chunk.crc = read_u32_be(buf+cursor+4+chunk.len);

    return chunk;
}



ImageFormat detectFormat(unsigned char* buf, long size) {
    if (size < 8) return FORMAT_UNKNOWN;

    if (memcmp(buf, PNG_SOI, 8) == 0) {
        return FORMAT_PNG;
    }
    if (buf[0] == JPEG_MARKER && buf[1] == SOI) {
        return FORMAT_JPEG;
    }

    return FORMAT_UNKNOWN;
}

char* format_bytes(long size) {
    char* str;

    if (size > 1024) sprintf(str, "%ld kb", size/1024);
    else sprintf(str, "%ld b", size);

    return str;
}

// to basically test if the bits work
void create_ppm(uint8_t *pixels, uint8_t bpp, ImageInfo info) {
    FILE *f = fopen("out.ppm", "wb");
    fprintf(f, "P6\n%u %u\n255\n", info.width, info.height);
    for (int i = 0; i<info.width*info.height; i++) {
        uint8_t r = pixels[i*bpp + 0];
        uint8_t g = pixels[i*bpp + 1];
        uint8_t b = pixels[i*bpp + 2];
        fwrite(&r, 1, 1, f);
        fwrite(&g, 1, 1, f);
        fwrite(&b, 1, 1, f);
    }
    fclose(f);
}

uint8_t paeth(uint8_t a, uint8_t b, uint8_t c) {
    int p = a + b - c;
    int pa = abs(p - a);
    int pb = abs(p - b);
    int pc = abs(p - c);

    if (pa <= pb && pa <= pc) return a;
    if (pb <= pc) return b;
    return c;
}

// returns filtered pixels from the filtered buf
uint8_t* filter(uint8_t *filtered_buf, uint8_t bpp, ImageInfo info) {
    printf("bpp in filter func: %u\n", bpp);
    uint8_t *pixels = malloc(info.width * info.height * bpp);


    for (int y = 0; y < info.height; y++) {
        uint8_t *row = filtered_buf + y * (1 + info.width * bpp);
        uint8_t *out = pixels + y * info.width * bpp;
        uint8_t filter = row[0];
        uint8_t *src = row + 1;

        for (int x = 0; x < info.width * bpp; x++) {
            uint8_t a = (x >= bpp) ? out[x - bpp] : 0;
            uint8_t b = (y > 0) ? (out-info.width*bpp)[x] : 0;
            uint8_t c = (x >= bpp && y > 0) ? (out-info.width*bpp)[x - bpp] : 0;

            switch (filter) {
                case 0: out[x] = src[x]; break;
                case 1: out[x] = src[x] + a; break;
                case 2: out[x] = src[x] + b; break;
                case 3: out[x] = src[x] + (a+b) / 2; break;
                case 4: out[x] = src[x] + paeth(a, b, c); break;
            }
        }
    }

    return pixels;
}

void print_info(ImageInfo info) {
    printf("format:   %s\n",   info.format == FORMAT_PNG ? "PNG" : "Cursed PNG format");
    printf("width:    %u\n",   info.width);
    printf("height:   %u\n",   info.height);
    printf("depth:    %u\n",   info.bit_depth);
    printf("interlace: %u\n", info.interlace);
    printf("channels: %u\n",   info.channels);
}


// gotta free pixels after you use them
uint8_t* process_png(unsigned char* buf, long size) {
    uint8_t *idat_buf = malloc(size);
    uint32_t idat_size = 0;

    ImageInfo info = get_png_info(buf, size);
    print_info(info);

    for (long i = IHDR_END; i < size ; i++) {
        // put chunk processing here
        if (memcmp(buf+i, IDAT, 4) == 0) {
            ImageChunk chunk = get_png_data(buf, i);
            memcpy(idat_buf+idat_size, chunk.data, chunk.len);

            idat_size += chunk.len;
        }

        if (memcmp(buf+i, IEND, 4) == 0) {
            printf("IEND encountered at address: %lu\n", i);
        }
    }

    uint8_t bpp = info.channels;
    size_t filtered_size = (size_t)info.height * (1 + (size_t)info.width * bpp); 
    uint8_t *filtered_buf = malloc(filtered_size);

    z_stream z = {0};

    inflateInit(&z);
    z.next_in = idat_buf;
    z.avail_in = idat_size;
    z.next_out = filtered_buf;
    z.avail_out = filtered_size;
    inflate(&z, Z_FINISH);
    inflateEnd(&z);

    printf("bpp:          %u\n", bpp);
    printf("filtered_size:%lu\n", filtered_size);
    printf("bytes written by inflate: %lu\n", filtered_size - z.avail_out);
    printf("first filter byte: %u\n", filtered_buf[0]);


    if (filtered_buf[0] > 4) {
        printf("ERR during inflating the idat buf - filter type is too big: %u\n", filtered_buf[0]);
    }

    // you gotta free pixels after you use them
    uint8_t *pixels = filter(filtered_buf, bpp, info);

    free(idat_buf);
    free(filtered_buf);

    create_ppm(pixels, bpp, info);

    return pixels;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: ./loader <path>\n");
        return 1;
    }

    char *path = argv[1];
    printf("loading: %s\n", path);

    FILE *f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    unsigned char *buf = malloc(size);
    fread(buf, 1, size, f);
    fclose(f);

    if (buf != NULL) {
        ImageFormat format = detectFormat(buf, size);
        if (format == FORMAT_PNG){
            uint8_t *pixels = process_png(buf, size);
            free(pixels);
        } else if (format == FORMAT_JPEG) {
            
        }else {
            printf("format is unknown\n");
        }

    }

    free(buf);


    return 0;
}
