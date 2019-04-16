/*
 * cc3circle.c
 *
 *  Created on: Apr 7, 2019
 *      Author: jmiller
 */

#include "os.h"
#include "cc3gfx.h"
#include "cc3raster.h"
#include "cc3circle.h"

// Draw circle using the midpoint algorithm.
void circle(int16_t xc, int16_t yc, int16_t radius, bool fill) {
    if (radius <= 0) {
        setPixel(xc, yc);
        return;
    }

    int16_t x = radius, y = 0;
    int16_t midpoint = 1 - radius;
    do {
        int16_t x2 = -x + xc;
        int16_t x4 = -y + xc;
        int16_t y1 = y + yc;
        int16_t y2 = -y + yc;
        int16_t y3 = x + yc;
        int16_t y4 = -x + yc;
        if (fill) {
            fillPixels(x2, y1, x<<1);
            fillPixels(x2, y2, x<<1);
            fillPixels(x4, y3, y<<1);
            fillPixels(x4, y4, y<<1);
        } else {
            int16_t x1 = x + xc;
            int16_t x3 = y + xc;
            setPixel(x1, y1);
            setPixel(x2, y1);
            setPixel(x1, y2);
            setPixel(x2, y2);
            setPixel(x3, y3);
            setPixel(x4, y3);
            setPixel(x3, y4);
            setPixel(x4, y4);
        }
        y++;
        if (midpoint <= 0) { // midpoint inside or on the perimeter
            midpoint = midpoint + (y << 1) + 1;
        } else { // midpoint outside the perimeter
            x--;
            midpoint = midpoint + ((y - x) << 1) + 1;
        }
    } while (x >= y);
}
