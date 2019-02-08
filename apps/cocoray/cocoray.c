/*
 * intray.c
 *
 *  Created on: Dec 9, 2018
 *      Author: jmiller
 */

#include "os.h"
#include "isphere.h"
#include "icamera.h"
#include "imath.h"
#include "cocogfx.h"
#include "dither.h"
#include "raytrace.h"
#include "testscene.h"

const int WIDTH = 320; // Render width / height
const int HEIGHT = 200;
const int COLORS = 32;
const char GFXMODE = HS320x192x16;
const int DPYWIDTH = 320;
const int DPYHEIGHT = 200;
const int DACBITS = 2;

#define RBITS 1
#define GBITS 2
#define BBITS 1

#define MAXDEPTH 3 // depth for reflected rays

// Enables 6309 Native mode for higher speed
void set6309Native() {
    asm {
        ldmd  #$01
    }
}

void makeRgbPalette(const int DACBITS) {
    int sr = DACBITS - RBITS;
    int sg = DACBITS - GBITS;
    int sb = DACBITS - BBITS;
    for (uint8_t i = 0; i < 16; i++) {
        uint8_t r = i >> (GBITS + BBITS);
        uint8_t g = (i >> BBITS) & ((1 << GBITS) - 1);
        uint8_t b = i & ((1 << BBITS) - 1);
        paletteRGB(i, r << sr, g << sg, b << sb);
        printf("Pallete(%d, %d %d %d)\n", i, r << sr, g << sg, b << sb);
    }
}

uint8_t ditherRGB(int i, int j, Vec3i* color) {
    uint8_t red = (uint8_t) (color->x >> (fraction + 1 - 8)); /* 0..255 */
    red = dither(8, RBITS, (uint8_t) i, (uint8_t) j, red);
    uint8_t grn = (uint8_t) (color->y >> (fraction + 1 - 8)); /* 0..255 */
    grn = dither(8, GBITS, (uint8_t) i, (uint8_t) j, grn);
    uint8_t blu = (uint8_t) (color->z >> (fraction + 1 - 8)); /* 0..255 */
    blu = dither(8, BBITS, (uint8_t) i, (uint8_t) j, blu);
    return (red << (GBITS + BBITS)) | (grn << BBITS) | blu;
}

static Vec3i palette[16]; // 16-color table entries

void makeSimplePalette() {
    for (uint8_t i = 0; i < 4; i++) {
        int val = i << 6;
        ivec3(val, 0, 0, &palette[i]); // red
        ivec3(0, val, 0, &palette[i + 4]); // green
        ivec3(0, 0, val, &palette[i + 8]); // blue
        ivec3(val, val, val, &palette[i + 12]); // gray
    }

    for (uint8_t i = 0; i < 16; i++) {
        uint8_t red = (uint8_t) (palette[i].x >> 6);
        uint8_t grn = (uint8_t) (palette[i].y >> 6);
        uint8_t blu = (uint8_t) (palette[i].z >> 6);
        paletteRGB(i, red, grn, blu);
    }
}

uint8_t nearest(Vec3i* color) {
    uint8_t index = 0;
    const uint8_t shift = (fraction + 1 - 8);
    Vec3i scaled;
    scaled.x = color->x >> shift; /* 0..255 */
    scaled.y = color->y >> shift; /* 0..255 */
    scaled.z = color->z >> shift; /* 0..255 */
    fixed best = 0x7fff; // TODO: base on sizeof(fixed)
    for (uint8_t i = 0; i < 16; i++) {
        Vec3i diff;
        isub3(&scaled, &palette[i], &diff);
        imult3s(&diff, c_half, &diff); // avoid overflow into sign bit
        fixed dist = idot3(&diff, &diff);
        if (dist < best) {
            index = i;
            best = dist;
        }
    }
    return index;
}

static Vec3i error;
uint8_t diffusion(Vec3i* color, int reset) {
    if (reset) {
        error.x = error.y = error.z = c_zero;
    }
    const uint8_t shift = (fraction + 1 - 8);
    Vec3i adjColor;
    iadd3(color, &error, &adjColor);
    Vec3i clamped;
    clamped.x = clamp(adjColor.x, 0, c_one);
    clamped.y = clamp(adjColor.y, 0, c_one);
    clamped.z = clamp(adjColor.z, 0, c_one);

    uint8_t index = nearest(&clamped);
    Vec3i chosen;
    chosen.x = palette[index].x << shift;
    chosen.y = palette[index].y << shift;
    chosen.z = palette[index].z << shift;
    isub3(color, &chosen, &chosen);
    iadd3(&error, &chosen, &error);
    error.x = clamp(error.x, -c_one, c_one);
    error.y = clamp(error.y, -c_one, c_one);
    error.z = clamp(error.z, -c_one, c_one);
    return index;
}

int main(int argc, char** argv) {
    /* Speedups */
    set6309Native();
    initCoCoSupport();
    setHighSpeed(1);

    /* Graphics */
    hscreen(GFXMODE);
    //makeRgbPalette(DACBITS);
    makeSimplePalette();

    /* Create the scene */
    float aspect = (float) WIDTH / HEIGHT;
    Scene *scene = testScene(fromFloat(aspect));

    for (int j = 0; j < HEIGHT; j++) {
        fixed v = c_one - (fixed) ((fresult) c_one * j / HEIGHT);
        for (int i = 0; i < WIDTH; i++) {
            iRay ray;
            Vec3i color;
            iHit hit;
            fixed u = (fixed) ((fresult) c_one * i / WIDTH);
            hset(i, j, 15); // show which pixel we're working on
            cam_makeRay(scene->camera, u, v, &ray);
            init_hit(&hit);
            ivec3(0,0,0, &color);
            if (trace(scene, &ray, &hit)) {
                shade(scene, &ray, &hit, &color, MAXDEPTH);
            } else {
                color = scene->background;
            }
            color.x = clamp(color.x, 0, c_one);
            color.y = clamp(color.y, 0, c_one);
            color.z = clamp(color.z, 0, c_one);
            //hset(i, j, ditherRGB(i, j, &color));
            //hset(i, j, nearest(&color));
            hset(i, j, diffusion(&color, i == 0));
        }
    }

    while (1)
        ;

    return 0;
}
