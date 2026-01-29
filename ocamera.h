#ifndef _MICROOCAMERA_H
#define _MICROOCAMERA_H

#include "./types.h"

// Define an O(rthographic) CAMERA

// Type definitions for the orthagraphic camera
typedef struct m3_ocamera_t {
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

    // Width/height of scene for camera to capture
    uint8_t width;
    uint8_t height;
} m3_ocamera_t;

typedef m3_ocamera_t* m3_ocamera_handle_t;

// Create, destroy, position, pivot, attach

// -------- BOOK KEEPING --------

/**
 * Create a camera at (0,0,0) with the specified width/height to capture
 * @param width     The width of the scene to capture
 * @param height    The height of the scene to capture
 * @returns         A handle to reference this camera later
 * NULL is returned if the camera could not be created (unable to malloc space for camera data)
 */
m3_ocamera_handle_t m3_ocamera_create(uint8_t width, uint8_t height);

// -------- CAMERA POSITION --------

/**
 * Set the position of an camera in 3d space.
 * @param object    The handle of the camera to modify
 * @param vec       The vector determining the new position
 * @returns         Whether the operation was successful
 * On failure, the original camera is unmodified
 * M3_SUCCESS: Success
object/segment * M3_ERR_EXIST_A: The camera does not exist
 */
m3_err_t me_ocamera_position(m3_ocamera_handle_t camera, m3_vec vec);

/**
 * Set the rotation of the camera in 3d space.
 * @param object    The handle of the camera to modify
 * @param quat      The quaternion determining the new orientation
 * @returns         Whether the operation was successful
 * On failure, the original camera is unmodified
 * M3_SUCCESS: Success
 * M3_ERR_EXIST_A: The camera does not exist
 */
m3_err_t me_ocamera_pivot(m3_ocamera_handle_t camera, m3_quat vec);



#endif