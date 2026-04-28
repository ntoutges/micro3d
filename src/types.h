#ifndef _MICROTYPES_H
#define _MICROTYPES_H

#include <stdint.h>
#include <stdbool.h>

// Common types used throught the Micro3D library

// Helpful 3d structs
typedef struct m3_vec {
    int8_t x;
    int8_t y;
    int8_t z;
} m3_vec;

typedef struct m3_quat {
    int8_t x;
    int8_t y;
    int8_t z;
    int8_t w;
} m3_quat;

// Segment data
typedef struct m3_segment_t {
    
    // Relative offsets from previous segments
    int8_t x: 4;
    int8_t y: 4;
    int8_t z: 4;

    // If 0: This segment is purely for positional movements
    uint8_t visible: 1;

    // If 1: This segment is positioned relative to its parent object, rather than the previous segment
    uint8_t absolute: 1;

    uint8_t unused: 1;

    // Always true: Marks a slot in the object segment heap as occupied
    uint8_t _marker: 1;
} m3_segment_t;

// Object data
typedef struct m3_object_t {
    // Position of the object
    m3_vec pos;

    // Orientation of the object
    // Garunteed to be stored a a unit quat.
    // Values +/- 127 represent a `1`
    m3_quat quat;

    // Id of parent
    // If set to self: no parent
    uint8_t parent;

    uint8_t scap: 3; // (S)mall (CAP)acity Number of segments able to be used in buf; Stores log2(capacity)
                     // Special case: scap = 0b111 -> capacity = 0
    uint8_t smem: 1; // (S)tatic (MEM)ory If set: segments stored in fixed-size array, else dynamically allocated
    uint8_t _marker: 1; // Always non-0: Marks a slot on the scene object heap as occupied
    
    uint8_t visible: 1; // Marks whether this object is visilbe (1) or hidden.
                        // If hidden: all segments and child objects will be hidden
    
    uint8_t lx: 1;      // Lock child segment rotation in the x-axis; Note that this does _not_ affect child objects
    uint8_t ly: 1;      // Lock child segment rotation in the y-axis; Note that this does _not_ affect child objects

    uint8_t soffset; // Offset from capacity, where `size = 2**scap - soffset`

    m3_segment_t* segments; // Segment buffer
} m3_object_t;

typedef struct m3_scene_t {
    uint8_t obj_size; // Size of the object buffers. If 0: buffer holds 256 entries
    m3_object_t* obj_buf; // Object memory; _UP TO_ 256 entries
} m3_scene_t;

typedef m3_scene_t* m3_scene_handle_t;

typedef struct m3_object_handle_t {
    m3_scene_handle_t owner;
    uint8_t id;
} m3_object_handle_t;

typedef struct m3_segment_handle_t {
    m3_object_handle_t owner;
    uint8_t id;
} m3_segment_handle_t;


// Error definitions
typedef enum m3_err_t {
    M3_SUCCESS = 0, // No error!
    M3_ERR_ALLOC, // Unable to allocate space
    M3_ERR_EXIST_A, // Some entity (main) does not exist
    M3_ERR_EXIST_B, // Some entity (secomdary) does not exist
    M3_ERR_BOUNDS,  // Invalid bounds
    M3_ERR_POS,     // Invalid location
    M3_ERR_CIRCULAR // Invalid hierarchy creation
} m3_err_t;

// Camera orientations
#define M3_ORIENTATION_HL    0b0000 // Standard bit ordering (bits indexed by x % 8)
#define M3_ORIENTATION_VL    0b0001 // Bits indexed by y % 8
#define M3_ORIENTATION_HFLIP 0b0010 // Reverse order of bits along the x-axis
#define M3_ORIENTATION_VFLIP 0b0100 // Reverse order of bits along the y-axis

#define M3_RLOCK_NONE 0b000 // All rotation allowed
#define M3_RLOCK_X    0b001 // Ignore rotation about the x-axis
#define M3_RLOCK_Y    0b010 // Ignore rotation about the y-axis

#endif