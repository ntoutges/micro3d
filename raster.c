#include "raster.h"

void _m3_raster_put_px(uint8_t* target, uint8_t width, uint8_t height, uint8_t x, uint8_t y);

// Integer counter algorithm
// Premise: Keep track of 2 counters: cx, cy, initially 0
// Runs off the principle attempting to satisfy Dx * dx = Dy * dy, where:
// Dx/Dy: Approximated line created by rasterizer
// dx/dy: Actual desired line slope
// If cx < cy: Move along the x-axis, increment cx by dy
// If cy < cx: Move along the y-axis, increment cy by dx
// If cx = cy: Move along the 'default' axis
// --- If |dx| >= |dy|: default axis = 'x'
// --- else: default axis = 'y'
void m3_raster_line(
    uint8_t* target,
    uint8_t width,
    uint8_t height,
    uint8_t x0,
    uint8_t y0,
    uint8_t x1,
    uint8_t y1
) {
    int8_t sx = x1 > x0 ? 1 : -1;
    int8_t sy = y1 > y0 ? 1 : -1;

    uint8_t abs_dx = (x1 > x0 ? x1 - x0 : x0 - x1);
    uint8_t abs_dy = (y1 > y0 ? y1 - y0 : y0 - y1);

    bool default_x = abs_dx >= abs_dy;

    uint16_t cx = 0;
    uint16_t cy = 0;

    // Set initial bit at x0,y0
    _m3_raster_put_px(target, width, height, x0, y0);

    // Note: Current position stored in x0,y0
    // Loop until end is reached
    while (x0 != x1 || y0 != y1) {
        
        // Move along x-axis
        if (cx < cy || (cx == cy && default_x)) {
            cy -= cx;
            cx = abs_dy;
            x0 += sx;
        }

        // Move along y-axis
        else {
            cx -= cy;
            cy = abs_dx;
            y0 += sy;
        }

        // Set bit at x0,y0
        _m3_raster_put_px(target, width, height, x0, y0);
    }
}

inline void _m3_raster_put_px(uint8_t* target, uint8_t width, uint8_t height, uint8_t x, uint8_t y) {
    // Calculate index given width/height
    uint8_t index = (y*width + x) / 8;
    uint8_t mask = 0b00000001 << (x % 8);

    // Update target at the specified index/bit
    target[index] |= mask;
}