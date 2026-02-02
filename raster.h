#ifndef _MICRORASTER_H
#define  _MICRORASTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

// Bounding box
typedef struct m3_bb {
    int8_t x_min;
    int8_t y_min;
    int8_t x_max;
    int8_t y_max;
} m3_bb;

/**
 * Render a line segment to some buffer target
 * Each rendered pixel is a single bit, where 1 byte makes up a _row_ (left->right) of pixels
 * Pixels are rendered in the area (0, 0) -> (width, height), all others are ignored
 * @param target    The target to render to
 * @param width     The number of pixels in width. If not a multiple of 8: rounds to next value
 * @param height    The number of pixels in height
 * @param x0        The first x-coorinate of the line to draw
 * @param y0        The first y-coorinate of the line to draw
 * @param x1        The second x-coorinate of the line to draw
 * @param y1        The second y-coorinate of the line to draw
 * @param orientation   The orientation mode to render the buffer. Union multiple compatible options for combinations
 * M3_ORIENTATION_HL: Target format is inferred to be `target[y][x/8]:x%8`
 * M3_ORIENTATION_VL: Target format is inferred to be `target[y/8][x]:y%8` (ex: SSD1306 format)
 * M3_ORIENTATION_HFLIP: Render image flipped about the y-axis
 * M3_ORIENTATION_VFLIP: Render image flipped about the x-axis
 */
void m3_raster_line(
    uint8_t* target,
    uint8_t width,
    uint8_t height,
    int16_t x0,
    int16_t y0,
    int16_t x1,
    int16_t y1,
    uint8_t orientation
);

/**
 * Get the bounding box of a set of arbitrary points
 * @param x1 The first point's x-position
 * @param y1 The first point's y-position
 * @param x2 The second point's x-position
 * @param y2 The second point's y-position
 * @returns The created bounding box
 */
m3_bb m3_raster_bb(int8_t x1, int8_t y1, int8_t x2, int8_t y2);

/**
 * Check if two bounding boxes intersect
 * @param a A bounding box
 * @param b A bounding box
 * @returns Whether the bounding boxes intersect
 */
bool m3_raster_bb_isect(m3_bb a, m3_bb b);

#ifdef __cplusplus
}
#endif

#endif