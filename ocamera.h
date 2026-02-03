#ifndef _MICROOCAMERA_H
#define _MICROOCAMERA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"

#include "types.h"
#include "object.h"
#include "segment.h"
#include "objpos.h"
#include "raster.h"

// Define an O(rthographic) CAMERA

// Type definitions for the orthagraphic camera
typedef struct m3_ocamera_t {
    // Position of the camera
    m3_vec pos;

    // Orientation of the camera
    // Garunteed to be stored a a unit quat.
    // Values +/- 127 represent a `1`
    m3_quat quat;

    // Width/height of scene for camera to capture
    uint8_t width;
    uint8_t height;
} m3_ocamera_t;

typedef m3_ocamera_t* m3_ocamera_handle_t;

// Create, destroy, position, pivot, attach

// -------- BOOK KEEPING --------

/**
 * Create a camera at (0,0,0) with the specified width/height to capture
 * @returns         A handle to reference this camera later
 * NULL is returned if the camera could not be created (unable to malloc space for camera data)
 */
m3_ocamera_handle_t m3_ocamera_create_d();

/**
 * Create a camera at (0,0,0) with the specified width/height to capture
 * @param camera    The camera struct to populate
 * @returns         A handle to reference this camera later
 */
m3_ocamera_handle_t m3_ocamera_create_s(m3_ocamera_handle_t camera);

// -------- CAMERA POSITION --------

/**
 * Set the position of an camera in 3d space.
 * Places the _center_ of the camera at this location
 * @param object    The handle of the camera to modify
 * @param vec       The vector determining the new position
 * @returns         Whether the operation was successful
 * On failure, the original camera is unmodified
 * M3_SUCCESS: Success
object/segment * M3_ERR_EXIST_A: The camera does not exist
 */
m3_err_t m3_ocamera_position(m3_ocamera_handle_t camera, m3_vec vec);

/**
 * Set the rotation of the camera in 3d space.
 * @param camera    The handle of the camera to modify
 * @param quat      The quaternion determining the new orientation
 * @returns         Whether the operation was successful
 * On failure, the original camera is unmodified
 * M3_SUCCESS: Success
 * M3_ERR_EXIST_A: The camera does not exist
 */
m3_err_t m3_ocamera_pivot(m3_ocamera_handle_t camera, m3_quat quat);

/**
 * Set the worldspace width/height of the scene captured by the camera
 * @param camera    The handle of the camera to modify
 * @param width     The new width of the camera's captured scene
 * @param width     The new height of the camera's captured scene
 * @returns         Whether the operation was successful
 * On failure, the original camera is unmodified
 * M3_SUCCESS: Success
 * M3_ERR_EXIST_A: The camera does not exist
 */
m3_err_t m3_ocamera_resize(m3_ocamera_handle_t camera, uint8_t width, uint8_t height);

/**
 * Render a scene to some target
 * @param camera    The handle of the camera to render using
 * @param scene     The handle of the scene to render
 * @param target    The target buffer to render into
 * @param width     The width of the target buffer to render into, in bits
 * @param height    The height of the target buffer to render into, in bits
 * @param orientation   The orientation mode to render the buffer. Union multiple compatible options for combinations
 * M3_ORIENTATION_HL: Target format is inferred to be `target[y][x/8]:x%8`
 * M3_ORIENTATION_VL: Target format is inferred to be `target[y/8][x]:y%8` (ex: SSD1306 format)
 * M3_ORIENTATION_HFLIP: Render image flipped about the y-axis
 * M3_ORIENTATION_VFLIP: Render image flipped about the x-axis
 * @returns         Whether the operation was successful
 * On failure, the original camera is unmodified
 * M3_SUCCESS: Success
 * M3_ERR_EXIST_A: The camera does not exist
 * M3_ERR_EXIST_B: The scene does not exist
 */
m3_err_t m3_ocamera_render(
    m3_ocamera_handle_t camera,
    m3_scene_handle_t scene,
    uint8_t* target,
    uint8_t width,
    uint8_t height,
    uint8_t orientation
);

#ifdef __cplusplus
}
#endif

#endif