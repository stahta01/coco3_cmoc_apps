/*
 * cc3rect.c
 *
 *  Created on: Mar 30, 2019
 *      Author: jmiller
 */

#include "os.h"
#include "cc3gfx.h"
#include "cc3util.h"
#include "cc3rect.h"
#include "cc3line.h"

void rect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color, bool fill)
{
    if (fill) {
        int16_t xmin = min(x0, x1);
        int16_t xmax = max(x0, x1);
        int16_t ymin = min(y0, y1);
        int16_t ymax = max(y0, y1);
        do {
            fillPixels(xmin, ymin, color, xmax - xmin);
            ymin++;
        } while (ymin < ymax);
    } else {
        line(x0, y0, x1, y0, color);
        line(x0, y1, x1, y1, color);
        line(x0, y0, x0, y1, color);
        line(x1, y0, x1, y1, color);
    }
}

