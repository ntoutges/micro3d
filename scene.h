#ifndef _MICROSCENE_H
#define _MICROSCENE_H

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "./types.h"

/**
 * Create a new scene using dynamic allocation
 * @param seg_size  The number of segments to allocate space for. If 0: Creates 256 slots
 * @param obj_size  The number of segments to allocate space for. If 0: Creates 256 slots
 * @returns         A handle to reference the scene later. If scene could not be created, returns NULL
 */
m3_scene_handle_t m3_scene_create_d(
    uint8_t seg_size,
    uint8_t obj_size
);

/**
 * Create a new scene using a static buffer
 * @param seg_buf The buffer used to to hold segments. Transfers ownership to the scene
 * @param seg_size The number of segments to allocate space for. If 0: Creates 256 slots
 * @param obj_buf The buffer used to to hold objects. Transfers ownership to the scene
 * @param obj_size The number of segments to allocate space for. If 0: Creates 256 slots
 * @returns         A handle to reference the scene later. If scene could not be created, returns NULL
 */
m3_scene_handle_t m3_scene_create_s(
    m3_segment_t* seg_buf,
    uint8_t seg_size,
    m3_object_t* obj_buf,
    uint8_t obj_size
);

#endif