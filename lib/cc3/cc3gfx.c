/*
 * cc3gfx.c
 *
 *  Created on: Feb 19, 2019
 *      Author: jmiller
 */

#include "os.h"
#include "cc3hw.h"
#include "cc3gfx.h"

// TODO: Define these by graphics mode...
#define BPP 4
#define PIXELSPERBYTE (8/BPP)
#define PAGEMASK 8191

#define CRES_1BPP 0
#define CRES_2BPP 1
#define CRES_4BPP 2

#define LPF_192 (0 << 5)
#define LPF_200 (1 << 5)
#define LPF_225 (3 << 5)

#define HRES7 (7 << 2) // 640x2bpp or 320x4bpp mode
#define HRES6 (6 << 2) // 512x2bpp or 256x4bpp mode
#define HRES5 (5 << 2) // 640x1bpp or 320x2bpp or 160x4bpp
#define HRES4 (4 << 2) // 512x1bpp or 256x2bpp
#define HRES2 (2 << 2) // 256x1bpp

static uint16_t widthPixels = 0;
static uint16_t heightPixels = 0;
static uint16_t bufferSizeBytes = 0;
static uint8_t bpp = 0;
static uint32_t gfxBase = 0x60000L;

// Constructs a palette entry of RGBRGB in the hardware format for Coco3.
uint8_t toPalette(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t result = 0;
    result = (result | ((r >> 1) & 1)) << 1;
    result = (result | ((g >> 1) & 1)) << 1;
    result = (result | ((b >> 1) & 1)) << 1;
    result = (result | (r & 1)) << 1;
    result = (result | (g & 1)) << 1;
    result = (result | (b & 1));
    return result;
}

int setMode(uint16_t xres, uint16_t yres, uint8_t depth) {
    uint8_t vrr = 0; // video resolution register setting when done
    switch (depth) {
        case 1: bpp = depth; vrr |= CRES_1BPP; bufferSizeBytes = xres * yres >> 3; break;
        case 2: bpp = depth; vrr |= CRES_2BPP; bufferSizeBytes = xres * yres >> 2; break;
        case 4: bpp = depth; vrr |= CRES_4BPP; bufferSizeBytes = xres * yres >> 1; break;
        default: return 0;
    }
    switch (xres) {
        case 160: widthPixels = 160; vrr |= (depth == 4 ? HRES6 : 0); break;
        case 256: widthPixels = 256; vrr |= (depth == 4 ? HRES6 : depth == 2 ? HRES4 : depth == 1 ? HRES2 : 0); break;
        case 320: widthPixels = 320; vrr |= (depth == 4 ? HRES7 : depth == 2 ? HRES5 : 0); break;
        case 640: widthPixels = 640; vrr |= (depth == 2 ? HRES7 : depth == 1 ? HRES5 : 0); break;
        default: return 0;
    }
    switch (yres) {
        case 192: heightPixels = 192; vrr |= LPF_192; break;
        case 200: heightPixels = 200; vrr |= LPF_200; break;
        case 225: heightPixels = 225; vrr |= LPF_225; break;
        default: return 0;
    }
    *videoResolution = vrr;
    *videoMode = 0x80; // graphics mode, 60Hz
    *init0 = 0x4c; // Coco3 mode & MMU enabled
    *vertScroll = 0; // clear vertical scroll register
    *vertOffset = gfxBase >> 3;

    return 1;
}

void clear(uint8_t color) {
    uint8_t pixels;
    switch (bpp) {
        case 1: pixels = color & 1;
                pixels |= pixels << 1;
                pixels |= pixels << 2;
                pixels |= pixels << 4;
                break;
        case 2: pixels = color & 3;
                pixels |= pixels << 2;
                pixels |= pixels << 4;
                break;
        case 4: pixels = color & 0x0f;
                pixels |= pixels << 4;
                break;
    }
    memset24(gfxBase, pixels, 0, bufferSizeBytes);
}

void setPixel(uint16_t x, uint16_t y, uint8_t clr) {
    uint8_t mask;
    uint8_t bit;
    uint8_t shift;
    uint32_t byteOffset;
    switch (bpp) {
        case 1:
            bit = 7 - ((uint8_t)x & 7); // pixels fill in from left
            clr = (clr & 1) << bit;
            mask = (1 << bit);
            byteOffset = (x + y * widthPixels) >> 3;
            break;
        case 2:
            bit = 3 - ((uint8_t)x & 3); // pixels fill in from left
            shift = bit << 1;
            clr = (clr & 0x3) << shift;
            mask = 0x3 << shift;
            byteOffset = (x + y * widthPixels) >> 2;
            break;
        case 4:
            bit = 1 - ((uint8_t)x & 1); // pixels fill in from left
            shift = bit << 2;
            clr = (clr & 0xf) << shift;
            mask = 0xf << shift;
            byteOffset = (x + y * widthPixels) >> 1;
            break;
    }
    memset24(gfxBase + byteOffset, clr, mask, 1);
}
