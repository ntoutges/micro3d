#ifndef _MICROSEGPOS_H
#define _MICROSEGPOS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdlib.h"
#include "stdint.h"

#include "types.h"
#include "transform.h"
#include "segment.h"

// Useful utility functions for getting the position of some object

// Type describing the transformation of an object
typedef struct m3_pos_object {
    m3_vec loc; // Location
    m3_quat rot; // Rotation
    m3_scene_t* owner;
} m3_pos_object;

// Return both a live (transformed) and static (untransformed) position, used for accurately constructing segment chains
typedef struct m3_pos_chain {
    m3_vec live;
    m3_vec sta;
} m3_pos_chain;

/**
 * Get the transformation of an object, factoring in all parent objects
 * Returns a default transformation on object non-existance
 * @param object    The object whose position will be retrieved
 * @returns         The final computed transformation
 */
m3_pos_object m3_pos_object_get(m3_object_handle_t object);

/**
 * Get the position of the end of a segment relative to some root
 * @param root      The root position/rotation
 * @param segment   The segment whose final position will be computed
 * @param scale     The scale factor to convert from segment space to world space. This is used to allow the small segment offsets to be computed with higher precision
 * @returns         The final computed position
 */
m3_pos_chain m3_pos_segment_get(m3_pos_object root, m3_segment_t segment, uint8_t scale);

/**
 * Get the position of the end of a segment relative to some root after some previous segment
 * @param root      The root position/rotation
 * @param segment   The segment whose final position will be computed
 * @param prev      The position of the previous segment
 * @param scale     The scale factor to convert from segment space to world space. This is used to allow the small segment offsets to be computed with higher precision
 * @returns         The final computed position
 */
m3_pos_chain m3_pos_segment_next(m3_pos_object root, m3_segment_t segment, m3_vec prev, uint8_t scale);


/**
 * Update a root to undo some given position/rotation transformation
 * This first reverses the rotation, then the position
 * Useful for transforming objects from global space to positions relative to a camera
 * Respects `rlock` property of the handle object
 * @param root  The object to modify
 * @param pos   The position portion of the transformation
 * @param quat  The rotation portion of the transformation. Must be normalized
 * @param object    The object whose rlock property will be used
 */
void m3_pos_root_reverse_rlock(m3_pos_object* root, m3_vec pos, m3_quat quat, m3_object_handle_t object);

/**
 * Update a root to undo some given position/rotation transformation
 * This first reverses the rotation, then the position
 * Useful for transforming objects from global space to positions relative to a camera
 * @param root  The object to modify
 * @param pos   The position portion of the transformation
 * @param quat  The rotation portion of the transformation. Must be normalized
 */
void m3_pos_root_reverse(m3_pos_object* root, m3_vec pos, m3_quat quat);

#ifdef __cplusplus
}
#endif

#endif