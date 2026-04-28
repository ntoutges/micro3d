#ifndef _MICROOBJECT_H
#define _MICROOBJECT_H

#include "stdio.h"
#include "stdlib.h"

#include "./types.h"
#include "./scene.h"
#include "segment.h"

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

/**
 * Use a static segment buffer to store segments within an object
 * This will copy as many segments as possible from `buf` into the object's segment list
 * @param object    The handle of the object to allocate segments for
 * @param buf       The buffer to use for segments
 * @param len       The length of the buffer. Note that this must be a power of 2.
 * If not: only the greatest power of 2 less than `len` will be used/available
 * @returns         Whether the operation was successful
 * On failure, the original object is unmodified
 * M3_SUCCESS: Success
 * M3_ERR_EXIST_A: The object does not exist
 * M3_ERR_EXIST_B: The objec's data is _not_ statically allocated
 */
m3_err_t m3_object_alloc_s(
    m3_object_handle_t object,
    m3_segment_t* buf,
    uint8_t len
);

/**
 * Switch an object from static segment memory to dynamic segment memory
 * This will copy all segments from the static buffer into a newly allocated dynamic buffer
 * @param object    The handle of the object to free segments for
 * @returns         Whether the operation was successful
 * On failure, the original object is unmodified
 * M3_SUCCESS: Success
 * M3_ERR_EXIST_A: The object does not exist
 * M3_ERR_ALLOC: Unable to allocate space for the new dynamic segment buffer
 */
m3_err_t m3_object_free_s(
    m3_object_handle_t object
);

// Result of object insertion
typedef struct m3_object_ires_t {
    uint8_t index; // The index of the insertion
    m3_err_t err; // The index of the removal
} m3_object_ires_t;

/**
 * Create a segment at the end of an object's segment list
 * @param object    The handle of the object to add the segment to
 * @param segment   The container to store the handle that references the created segment.
 * If the segment could not be created, this will _not_ be populated.
 * Pass in a value of `NULL` if the handle is undesired
 * @returns         The index of the new segment, and whether the operation was successful
 * On failure, the original object/segment are unmodified
 * M3_SUCCESS: Success
 * M3_ERR_ALLOC: Unable to allocate space for the new segment
 * M3_ERR_EXIST_A: The object does not exist
 */
m3_object_ires_t m3_object_push_segment(
    m3_object_handle_t object,
    m3_segment_handle_t* segment
);

/**
 * Create a segment in an object, and replace some preexisting
 * segment at the given indexwith the new segment
 * Note that this will invalidate any preexisting handles to all segments at the given index,
 * however (for memory and speed concerns) the handles will not reflect this.
 * 
 * @param object    The handle of the object to add the segment to
 * @param index     The location of the original segment to replace
 * This index must reference a created segment index, otherwise it will throw with M3_ERR_BOUNDS
 * @param segment   The container to store the handle that references the created segment.
 * If the segment could not be created, this will _not_ be populated.
 * Pass in a value of `NULL` if the handle is undesired
 * @returns         The index of the new segment, and whether the operation was successful
 * On failure, the original object/segment are unmodified
 * M3_SUCCESS: Success
 * M3_ERR_EXIST_A: The object does not exist
 * M3_ERR_BOUNDS: The given index is invalid
 */
m3_object_ires_t m3_object_replace_segment(
    m3_object_handle_t object,
    uint8_t index,
    m3_segment_handle_t* segment
);

/**
 * Create a segment in an object, and ripple insert into an object's segment list, offsetting
 * all other segments to make roomsome preexisting
 * Note that this will invalidate any handles to all segments at or after the
 * given index, however (for memory and speed concerns) the handles will not
 * reflect this.
 * 
 * @param object    The handle of the object to add the segment to
 * @param index     The location of the original segment to replace
 * This index must reference a created segment index, otherwise it will throw with M3_ERR_BOUNDS
 * @param segment   The container to store the handle that references the created segment.
 * If the segment could not be created, this will _not_ be populated.
 * Pass in a value of `NULL` if the handle is undesired
 * @returns         The index of the new segment, and whether the operation was successful
 * On failure, the original object/segment are unmodified
 * M3_SUCCESS: Success
 * M3_ERR_ALLOC: Unable to allocate space for the new segment
 * M3_ERR_EXIST_A: The object does not exist
 * M3_ERR_BOUNDS: The given index is invalid
 */
m3_object_ires_t m3_object_insert_segment(
    m3_object_handle_t object,
    uint8_t index,
    m3_segment_handle_t* segment
);

/**
 * Remove the segment at the given index, and ripple move all following
 * segments back to fill the gap.
 * Note that this will invalidate any handles to all segments at or after the
 * given index, however (for memory and speed concerns) the handles will not
 * reflect this.
 * 
 * @param object    The handle of the object to add the segment to
 * @param index     The index of the segment to remove from the object
 * @returns         Whether the operation was successful
 * On failure, the original object/segment are unmodified
 * M3_SUCCESS: Success
 * M3_ERR_EXIST_A: The object does not exist
 * M3_ERR_BOUNDS: The given index is invalid
 */
m3_err_t m3_object_remove_segment(
    m3_object_handle_t object,
    uint8_t index
);

/**
 * Clear all segment data associated with an object.
 * Note that ths will invalidate all handles to all segments in this
 * object, however (for memory and speed concenrs) the handles will
 * not reflect this.
 * 
 * @param object    The handle of the object to clear
 * @returns         Whether the operation was successful
 * On failure, the original object/segment are unmodified
 * M3_SUCCESS: Success
 * M3_ERR_EXIST_A: The object does not exist
 */
m3_err_t m3_object_clear(m3_object_handle_t object);

// -------- OBJECT SEGMENT TRAVERSAL --------

/**
 * Get the number of segments an object has access to
 * @param object    The handle of the object to read
 * @returns         The number of segments this object has access to, in the range [0, 256]
 * Returns 0 if object doesn't exist
 */
uint16_t m3_object_segment_length(m3_object_handle_t object);

/**
 * Get the raw buffer of segment an object has access to. This becomes invalid as soon as the object's segment list is modified
 * @param object    The handle of the object to read
 * @returns         The buffer holding the segments
 * Returns nullptr if object doesn't exist
 */
m3_segment_t* m3_object_segment_buf(m3_object_handle_t object);

/**
 * Get the handle of some segment
 * @param object    The handle of the object which uses the segment
 * @param index     The index in the object where the segment is used
 * @returns         The segment handle. Note that this may not exist
 */
m3_segment_handle_t m3_object_segment_get(m3_object_handle_t object, uint8_t index);

// -------- OBJECT MODIFICATION --------

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

/**
 * Set an object's visibility.
 * If an object is hidden, all its segments and child objects will _also_ be hidden.
 * @param object    The handle of the object to modify
 * @param visible   Whether this object should be made visible or not
 * @returns         Whether the operation was successful
 * On failure, the original object is unmodified
 * M3_SUCCESS: Success
 * M3_ERR_EXIST_A: The object does not exist
 */
m3_err_t m3_object_visible(m3_object_handle_t object, bool visible);

/**
 * Set the axes in which this object is locked
 * Determines whether the camera's rotation in some axis factors into the rendering of the object.
 * @param object    The handle of the object to modify
 * @param rlock     The axes to lock rotation off of. Union multiple together for complex rotation locking
 * @returns         Whether the operation was successful
 * On failure, the original object is unmodified
 * M3_SUCCESS: Success
 * M3_ERR_EXIST_A: The segment does not exist
 */
m3_err_t m3_object_rlock(
    m3_object_handle_t object,
    uint8_t rlock
);

// -------- OBJECT GETTERS --------

/**
 * Get an object's immediate or inherited
 * @param object    The handle of the object to read
 * @param chain     Whether to include parent objects in the calculation. If true: The parent chain will be walked up to determine the ultimate visibility
 * @returns         The visibility of the object. If invalid data is found, `false` is returned.
 */
bool m3_object_visibility(m3_object_handle_t object, bool chain);


#endif