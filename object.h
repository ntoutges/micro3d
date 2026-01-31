#ifndef _MICROOBJECT_H
#define __MICROOBJECT_H

#include "stdio.h"
#include "stdlib.h"

#include "./types.h"
#include "./scene.h"

// -------- BOOK-KEEPING FUNCTIONS --------

/**
 * Create a new object in a scene
 * @param scene The scene to create this object in
 * @returns     A handle to reference this object later
 * Note that the object may not be created if no space is left in object memory
 * Use `m3_object_exists` to verify existance
 */
m3_object_handle_t m3_object_create(m3_scene_handle_t scene);

/**
 * Remove an object from its scene
 * @param object The handle of the object to destroy
 */
void m3_object_destroy(m3_object_handle_t* object);

/**
 * Check if an object actually exists
 * @param object    The handle of the object to check the existance of
 * @returns         `true` if the object exists, `false` otherwise
 */
bool m3_object_exists(m3_object_handle_t object);

/**
 * Get the owning scene of an object
 * @param object    The handle of the object to get the owner of
 * @returns         The handle of the owner. If the object does not exist, a value of `NULL` is returned
 */
m3_scene_handle_t m3_object_owner(m3_object_handle_t object);

// -------- OBJECT/SEGMENT MODIFICATION --------

// Result of object insertion
typedef struct m3_object_ires_t {
    uint8_t index; // The index of the insertion
    m3_err_t err; // The index of the removal
} m3_object_ires_t;

/**
 * Add a segment at the end of an object's segment list
 * @param object    The handle of the object to add the segment to
 * @param segment   The handle of the segment to add to the object
 * @returns         The index of the new segment, and whether the operation was successful
 * On failure, the original object/segment are unmodified
 * M3_SUCCESS: Success
 * M3_ERR_ALLOC: Unable to allocate space for the new segment
 * M3_ERR_EXIST_A: The object does not exist
 * M3_ERR_EXIST_B: The segment does not exist, or has a different owner
 */
m3_object_ires_t m3_object_push_segment(
    m3_object_handle_t object,
    m3_segment_handle_t segment
);

/**
 * Replace the segment at the given index in an object's segment list
 * @param object    The handle of the object to add the segment to
 * @param segment   The handle of the segment to add to the object
 * @param index     The location of the original segment to replace
 * @returns         The index of the new segment, and whether the operation was successful
 * On failure, the original object/segment are unmodified
 * M3_SUCCESS: Success
 * M3_ERR_EXIST_A: The object does not exist
 * M3_ERR_EXIST_B: The segment does not exist, or has a different owner
 * M3_ERR_BOUNDS: The given index is invalid
 */
m3_object_ires_t m3_object_replace_segment(
    m3_object_handle_t object,
    m3_segment_handle_t segment,
    uint8_t index
);

/**
 * Ripple insert a segment into an object's segment list, offsetting all other segments to make room
 * @param object    The handle of the object to add the segment to
 * @param segment   The handle of the segment to add to the object
 * @param index     The location of the original segment to insert at
 * @returns         The index of the new segment, and whether the operation was successful
 * On failure, the original object/segment are unmodified
 * M3_SUCCESS: Success
 * M3_ERR_ALLOC: Unable to allocate space for the new segment
 * M3_ERR_EXIST_A: The object does not exist
 * M3_ERR_EXIST_B: The segment does not exist
 * M3_ERR_BOUNDS: The given index is invalid
 */
m3_object_ires_t m3_object_insert_segment(
    m3_object_handle_t object,
    m3_segment_handle_t segment,
    uint8_t index
);

/**
 * Remove the segment at the given index
 * @param object    The handle of the object to add the segment to
 * @param index     The handle of the segment to add to the object
 * @returns         Whether the operation was successful
 * On failure, the original object/segment are unmodified
 * M3_SUCCESS: Success
 * M3_ERR_ALLOC: Unable to allocate space for the new segment
 * M3_ERR_EXIST_A: The object does not exist
 * M3_ERR_BOUNDS: The given index is invalid
 */
m3_err_t m3_object_remove_segment(
    m3_object_handle_t object,
    uint8_t index
);

/**
 * Clear all segment data associated with an object
 * @param object    The handle of the object to clear
 * @returns         Whether the operation was successful
 * On failure, the original object/segment are unmodified
 * M3_SUCCESS: Success
 * M3_ERR_EXIST_A: The object does not exist
 */
m3_err_t m3_object_clear(m3_object_handle_t object);

/**
 * Set the parent of an object for hierarchy creation
 * @param object    The handle of the object to clear
 * @param parent    The handle of the object to set as the parent.
 * If object and parent are the same object, the parent is reset
 * @returns         Whether the operation was successful
 * On failure, the original object/segment are unmodified
 * M3_SUCCESS: Success
 * M3_ERR_EXIST_A: The object does not exist
 * M3_ERR_EXIST_B: The parent does not exist within `object`'s scene
 * M3_ERR_CIRCULAR: The created hierarchy contains a circular reference
 */
m3_err_t m3_object_pset(m3_object_handle_t object, m3_object_handle_t parent);

/**
 * Clear the parent of an object to reset the hierarchy
 * @param object    The handle of the object to clear
 * @returns         Whether the operation was successful
 * On failure, the original object/segment are unmodified
 * M3_SUCCESS: Success
 * M3_ERR_EXIST_A: The object does not exist
 */
m3_err_t m3_object_pclear(m3_object_handle_t object);

// -------- OBJECT SEGMENT TRAVERSAL --------

/**
 * Get the number of segments an object has access to
 * @param object    The handle of the object to read
 * @returns         The number of segments this object has access to, in the range [0, 256]
 * Returns 0 if object doesn't exist
 */
uint16_t m3_object_segment_length(m3_object_handle_t object);

/**
 * Get the raw buffer of segment ids an object has access to. This becomes invalid as soon as the object's segment list is modified
 * @param object    The handle of the object to read
 * @returns         The buffer holding the segment ids
 * Returns nullptr if object doesn't exist
 */
uint8_t* m3_object_segment_buf(m3_object_handle_t object);

/**
 * Get the handle of some segment
 * @param object    The handle of the object which uses the segment
 * @param index     The index in the object where the segment is used
 * @returns         The segment handle. Note that this may not exist
 */
m3_segment_handle_t m3_object_segment_get(m3_object_handle_t object, uint8_t index);

// -------- OBJECT POSITION --------

/**
 * Set the position of an object in 3d space.
 * @param object    The handle of the object to modify
 * @param vec       The vector determining the new position of the object
 * @returns         Whether the operation was successful
 * On failure, the original object/segment are unmodified
 * M3_SUCCESS: Success
 * M3_ERR_EXIST_A: The object does not exist
 */
m3_err_t m3_object_position(m3_object_handle_t object, m3_vec vec);

/**
 * Set the rotation of an object in 3d space.
 * @param object    The handle of the object to modify
 * @param quat      The quaternion determining the new orientation of the object
 * @returns         Whether the operation was successful
 * On failure, the original object/segment are unmodified
 * M3_SUCCESS: Success
 * M3_ERR_EXIST_A: The object does not exist
 */
m3_err_t m3_object_pivot(m3_object_handle_t object, m3_quat quat);

#endif