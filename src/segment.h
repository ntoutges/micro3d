#ifndef _MICROSEGMENT_H
#define _MICROSEGMENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "stdlib.h"

#include "./types.h"
#include "./scene.h"

// -------- BOOK-KEEPING FUNCTIONS --------

/**
 * Check if a segment actually exists.
 * If false, the segment may be referencing an object or segment index 
 * within an object that no longer exists.
 * @param segment   The handle of the segment to check the existance of
 * @returns         `true` if the segment exists, `false` otherwise
 */
bool m3_segment_exists(m3_segment_handle_t segment);

// -------- MODIFICATION --------

/**
 * Set the local x/y/z offset of the segment
 * @param segment   The handle of the segment to modify
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
 * @param segment   The handle of the segment to modify
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

/**
 * Set whether this segment is positioned absolute (relative to parent object, rather than previous segment)
 * @param segment   The handle of the segment to modify
 * @param absolute  `true` to mark this segment as absolutely positioned. `false` for relative positioning (default)
 * @returns         Whether the operation was successful
 * On failure, the original segment is unmodified
 * M3_SUCCESS: Success
 * M3_ERR_EXIST_A: The segment does not exist
 */
m3_err_t m3_segment_absolute(
    m3_segment_handle_t handle,
    bool absolute
);

// -------- GETTERS --------

/**
 * Get whether this segment is visible or not
 * @param segment   The handle of the segment to get the visibility state of
 * @returns         Whether the segment is visible. If the segment does not exist, a value of `false` is returned
 */
bool m3_segment_visibility(m3_segment_handle_t handle);

/**
 * Get the owning object of a segment
 * @param segment   The handle of the segment to get the owner of
 * @returns         The handle of the owner. If the segment does not exist, a handle to an invalid object
 * will be returned.
 */
m3_object_handle_t m3_segment_owner(m3_segment_handle_t segment);

#ifdef __cplusplus
}
#endif

#endif