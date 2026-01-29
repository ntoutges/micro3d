#ifndef _MICRORASTER_H
#define  _MICRORASTER_H

#include "types.h"

/**
 * Render a line segment to some buffer target
 * Each rendered pixel is a single bit, where 1 byte makes up a _row_ (left->right) of pixels
 * @param target    The target to render to
 * @param width     The number of pixels in width. If not a multiple of 8: rounds to next value
 * @param height    The number of pixels in height
 * @param x0        The first x-coorinate of the line to draw
 * @param y0        The first y-coorinate of the line to draw
 * @param x1        The second x-coorinate of the line to draw
 * @param y1        The second y-coorinate of the line to draw
 */
void m3_raster_line(
    uint8_t* target,
    uint8_t width,
    uint8_t height,
    uint8_t x0,
    uint8_t y0,
    uint8_t x1,
    uint8_t y1
);

#endif