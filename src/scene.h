#ifndef _MICROSCENE_H
#define _MICROSCENE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "./types.h"

/**
 * Create a new scene using dynamic allocation
 * @param obj_size  The number of objects to allocate space for. If 0: Creates 256 slots
 * @returns         A handle to reference the scene later. If scene could not be created, returns NULL
 */
m3_scene_handle_t m3_scene_create_d(
    uint8_t obj_size
);

/**
 * Create a new scene using a static buffer
 * @param scene     The scene struct to populate
 * @param obj_buf   The buffer used to to hold objects. Transfers ownership to the scene
 * @param obj_size  The number of segments to allocate space for. If 0: Creates 256 slots
 * @returns         A handle to reference the scene later. If scene could not be created, returns NULL
 */
m3_scene_handle_t m3_scene_create_s(
    m3_scene_t* scene,
    m3_object_t* obj_buf,
    uint8_t obj_size
);

/**
 * Get an object handle by its id
 * Note that this object is not garunteed to exist
 * 
 * @param scene The scene from which the object is searched
 * @param id    The id of the object within the scene tor grab
 * @return      The handle of the object, that may not exist
 */
m3_object_handle_t m3_scene_object_get(
    m3_scene_t* scene,
    uint8_t id
);

#ifdef __cplusplus
}
#endif

#endif