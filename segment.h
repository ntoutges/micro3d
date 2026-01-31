#ifndef _MICROSEGMENT_H
#define _MICROSEGMENT_H

#include "stdio.h"
#include "stdlib.h"

#include "./types.h"
#include "./scene.h"

// -------- BOOK-KEEPING FUNCTIONS --------

/**
 * Create a new _visible_ segment in a scene, with offset (0, 0, 0)
 * @param scene The scene to create this segment in
 * @returns     A handle to reference this segment later
 * Note that the segment may not be created, if no space is left in segment memory
 * Use `m3_segment_exists` to verify existance
 */
m3_segment_handle_t m3_segment_create(m3_scene_handle_t scene);

/**
 * Remove a segment from its scene. All objects pointing to this segment should first be redirected
 * @param segment The handle of the segment to destroy
 */
void m3_segment_destroy(m3_segment_handle_t* segment);

/**
 * Check if a segment actually exists
 * @param segment   The handle of the segment to check the existance of
 * @returns         `true` if the segment exists, `false` otherwise
 */
bool m3_segment_exists(m3_segment_handle_t segment);

/**
 * Get the owning scene of a segment
 * @param segment   The handle of the segment to get the owner of
 * @returns         The handle of the owner. If the segment does not exist, a value of `NULL` is returned
 */
m3_scene_handle_t m3_segment_owner(m3_segment_handle_t segment);

/**
 * Get a handle to a segment
 * @param owner The owning scene
 * @param id    The segment's id within the scene
 * @returns     The handle to the segment. Note that the segment may not exist.
 */
m3_segment_handle_t m3_segment_get(m3_scene_handle_t owner, uint8_t id);

// -------- MODIFICATION --------

/**
 * Set the local x/y/z offset of the segment
 * @param segment   The handle of the segment to get the owner of
 * @param vec       The offset that this segment represents
 * @returns         Whether the operation was successful
 * On failure, the original segment is unmodified
 * M3_SUCCESS: Success
 * M3_ERR_EXIST_A: The segment does not exist
 * M3_ERR_POS: The given vector is invalid (some component out-of-bounds)
 */
m3_err_t m3_segment_offset(
    m3_segment_handle_t handle,
    m3_vec vec
);

/**
 * Set whether this segment is visible or not
 * @param segment   The handle of the segment to get the owner of
 * @param visible   `true` to mark this segment as visible. `false` for purely positioning
 * @returns         Whether the operation was successful
 * On failure, the original segment is unmodified
 * M3_SUCCESS: Success
 * M3_ERR_EXIST_A: The segment does not exist
 */
m3_err_t m3_segment_visible(
    m3_segment_handle_t handle,
    bool visible
);

// -------- GETTERS --------

/**
 * Get whether this segment is visible or not
 * @param segment   The handle of the segment to get the owner of
 * @returns         Whether the segment is visible. If the segment does not exist, a value of `false` is returned
 */
bool m3_segment_visibility(m3_segment_handle_t handle);

#endif