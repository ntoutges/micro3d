#include "raster.h"

void _m3_raster_put_px(uint8_t* target, uint8_t width, uint8_t height, uint8_t x, uint8_t y);
bool _m3_raster_in_bounds(int16_t x, int16_t y, uint8_t width, uint8_t height);

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
    int16_t x0,
    int16_t y0,
    int16_t x1,
    int16_t y1
) {
    int8_t sx = x1 > x0 ? 1 : -1;
    int8_t sy = y1 > y0 ? 1 : -1;

    uint8_t abs_dx = (x1 > x0 ? x1 - x0 : x0 - x1);
    uint8_t abs_dy = (y1 > y0 ? y1 - y0 : y0 - y1);

    bool default_x = abs_dx >= abs_dy;

    uint16_t cx = 0;
    uint16_t cy = 0;

    bool in_bounds = _m3_raster_in_bounds(x0, y0, width, height);

    // Set initial bit at x0,y0
    if (in_bounds) _m3_raster_put_px(target, width, height, x0, y0);

    // Note: Current position stored in x0,y0
    // Loop until end is reached
    while (x0 != x1 || y0 != y1) {
        
        // Move along x-axis
        if (cx < cy || (cx == cy && default_x)) {
            cy -= cx;
            cx = abs_dy;
            x0 += sx;

            // Dropped out-of-bounds; Skip the rest!
            if (in_bounds && (x0 < 0 || x0 >= width)) break;
        }

        // Move along y-axis
        else {
            cx -= cy;
            cy = abs_dx;
            y0 += sy;
            
            // Dropped out-of-bounds; Skip the rest!
            if (in_bounds && (y0 < 0 || y0 >= height)) break;
        }

        // Currently searching for start of boundary; Don't bother rendering
        if (!in_bounds) {
            in_bounds = _m3_raster_in_bounds(x0, y0, width, height); // Check if we just got in bounds

            // Still out-of-bounds; Ignore render phase
            if (!in_bounds) continue;
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

inline bool _m3_raster_in_bounds(int16_t x, int16_t y, uint8_t width, uint8_t height) {
    return x >= 0 && y >= 0 && x < width && y < height;
}

m3_bb m3_raster_bb(int8_t x1, int8_t y1, int8_t x2, int8_t y2) {
    m3_bb bbox;

    if (x1 < x2) {
        bbox.x_min = x1;
        bbox.x_max = x2;
    }
    else {
        bbox.x_min = x2;
        bbox.x_max = x1;
    }

    if (y1 < y2) {
        bbox.y_min = y1;
        bbox.y_max = y2;
    }
    else {
        bbox.y_min = y2;
        bbox.y_max = y1;
    }

    return bbox;
}

bool m3_raster_bb_isect(m3_bb a, m3_bb b) {
    return a.x_max >= b.x_min
        && b.x_max >= a.x_min
        && a.y_max >= b.y_min
        && b.y_max >= a.y_min;
}

