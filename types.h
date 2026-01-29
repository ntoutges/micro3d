#ifndef _MICROTYPES_H
#define _MICROTYPES_H

#include <stdint.h>
#include <stdbool.h>

// Segment data
typedef struct m3_segment_t {
    
    // Relative offsets from previous segments
    int8_t x: 4;
    int8_t y: 4;
    int8_t z: 4;

    // If 0: This segment is purely for positional movements
    uint8_t visible: 1;

    uint8_t unused: 2;

    // Always true: Marks a slot in the scene segment heap as occupied
    uint8_t _marker: 1;
} m3_segment_t;

// Object data
typedef struct m3_object_t {
    // Position of the object
    int8_t x;
    int8_t y;
    int8_t z;

    // Orientation of the object
    // Garunteed to be stored a a unit quat.
    // Values +/- 127 represent a `1`
    int8_t qw;
    int8_t qx;
    int8_t qy;
    int8_t qz;

    // Id of parent
    // If set to self: no parent
    uint8_t parent;

    // Indices of segments this object is made of

    uint8_t scap: 4; // (S)mall (CAP)acity Number of segments able to be used in buf; Stores log2(capacity)
                     // Special case: scap = 0b1111 -> capacity = 0
    uint8_t _marker: 1; // Always non-0: Marks a slot on the scene object heap as occupied
    uint8_t unused: 3; // Unused

    uint8_t soffset; // Offset from capacity, where `size = 2**scap - soffset`

    uint8_t* segments; // Segment buffer; Dynamically allocated
} m3_object_t;

typedef struct m3_scene_t {
    
    // Size of the given buffers. If 0: buffer holds 256 entries
    uint8_t seg_size;
    uint8_t obj_size;

    // Segment memory; _UP TO_ 256 entries
    m3_segment_t* seg_buf;

    // Object memory; _UP TO_ 256 entries
    m3_object_t* obj_buf;
} m3_scene_t;

typedef m3_scene_t* m3_scene_handle_t;

typedef struct m3_segment_handle_t {
    m3_scene_handle_t owner;
    uint8_t id;
} m3_segment_handle_t;

typedef struct m3_object_handle_t {
    m3_scene_handle_t owner;
    uint8_t id;
} m3_object_handle_t;


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
typedef enum m3_camera_orientation_t {
    M3_HORIZONTAL,
    M3_VERTICAL,
    M3_HORIZONTAL_F,
    M3_VERTICAL_F
} m3_camera_orientation_t;

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

#endif