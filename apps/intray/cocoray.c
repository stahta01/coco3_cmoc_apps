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

const int WIDTH = 400;
const int HEIGHT = 200;
const int COLORS = 32;
const char GFXMODE = HS640x192x4;
const int DPYWIDTH = 640;
const int DPYHEIGHT = 200;

// Enables 6309 Native mode for higher speed
void set6309Native() {
    asm {
        ldmd  #$01
    }
}

int main(int argc, char** argv) {
    Vec3i from;
    Vec3i at;
    Vec3i up;
    iCamera cam;
    iSphere sp;

    /* Speedups */
    set6309Native();
    initCoCoSupport();
    setHighSpeed(1);

    /* Graphics */
    hscreen(GFXMODE);
    for (uint8_t i = 0; i < 4; i++) {
        paletteRGB(i, i, i, i);
    }

    /* Create the camera */
    fixed eye = toFixed(3.0f);
    ivec3(eye, eye, eye, &from);
    ivec3(c_zero, c_zero, c_zero, &at);
    ivec3(c_zero, c_one, c_zero, &up);
    cam_create(&from, &at, &up, toFixed(45.0f), toFixed((float) 1.0f), &cam);

    /* Create the scene */
    sp_create(c_zero, c_zero, c_zero, toFixed(0.1f), &sp);

    Vec3i lightdir;
    ivec3(toFixed(1), toFixed(1), toFixed(1), &lightdir);
    inormalize3(&lightdir);
    printf("Light dir: ");
    ivec3_print(&lightdir);
    fixed kdiff = c_half;
    for (int j = 0; j < HEIGHT; j++) {
        fixed v = (fixed) ((fresult) c_one * j / HEIGHT);
        for (int i = 0; i < WIDTH; i++) {
            fixed u = (fixed) ((fresult) c_one * i / WIDTH);
            iRay ray;
            fixed tmax = toFixed(31.0f);
            cam_makeRay(&cam, u, v, &ray);
            int color = 0;
            if (sp_isect(&sp, &ray, &tmax)) {
                Vec3i normal;
                sp_normal(&sp, &ray, tmax, &normal);
                fixed d = idot3(&lightdir, &normal);
                color = fmult(fmult(d, kdiff), COLORS << fraction) >> fraction;
            }
            color = dither(COLORS, 4, i, j, color);
            hset(i, j, color);
        }
    }
    while (1)
        ;
    return 0;
}
