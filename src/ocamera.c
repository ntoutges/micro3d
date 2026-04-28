#include "./ocamera.h"

m3_ocamera_handle_t m3_ocamera_create_d() {
    m3_ocamera_t* camera = (m3_ocamera_t*) malloc(sizeof(m3_ocamera_t));

    if (!camera) return NULL;

    // Rely on static handler for actual struct instantiation
    return m3_ocamera_create_s(camera);
}

m3_ocamera_handle_t m3_ocamera_create_s(m3_ocamera_t* camera) {
    camera->pos = (m3_vec){ 0, 0, 0 };
    camera->quat = (m3_quat){ 0, 0, 0, 127 };

    camera->width = 0;
    camera->height = 0;

    return camera;
}

m3_err_t m3_ocamera_position(m3_ocamera_handle_t camera, m3_vec vec) {
    if (!camera) return M3_ERR_EXIST_A;
    camera->pos = vec;
    return M3_SUCCESS;
}

m3_err_t m3_ocamera_pivot(m3_ocamera_handle_t camera, m3_quat quat) {
    if (!camera) return M3_ERR_EXIST_A;
    camera->quat = quat;
    return M3_SUCCESS;
}

m3_err_t m3_ocamera_resize(m3_ocamera_handle_t camera, uint8_t width, uint8_t height) {
    if (!camera) return M3_ERR_EXIST_A;

    camera->width = width;
    camera->height = height;

    return M3_SUCCESS;
}

m3_err_t m3_ocamera_render(
    m3_ocamera_handle_t camera,
    m3_scene_handle_t scene,
    uint8_t* target,
    uint8_t width,
    uint8_t height,
    uint8_t orientation
) {
    // Verify camera/scene existance
    if (!camera) return M3_ERR_EXIST_A;
    if (!scene) return M3_ERR_EXIST_B;

    // Loop through all available objects
    uint8_t obj_size = scene->obj_size;

    // Default object handle
    m3_object_handle_t object = { scene, 0 };

    // Bounding box in _cameraspace_ (centered at (0, 0)
    int8_t min_cam_x = 0 - camera->width / 2;
    int8_t min_cam_y = 0 - camera->height / 2;
    m3_bb camera_bb = {
        min_cam_x,
        min_cam_y,
        min_cam_x + camera->width,
        min_cam_y + camera->height,
    };

    // Used to center objects in the screen
    int8_t screen_offset_x = width / 2;
    int8_t screen_offset_y = height / 2;

    // Scale factor to convert from cameraspace to screenspace
    float x_scale = (float) width / camera->width;
    float y_scale = (float) height / camera->height;

    for (uint16_t i = 0; i < obj_size; i++) {
        m3_object_t* obj = &(scene->obj_buf[i]);

        // Object doesn't exist; Ignore!
        if (!obj->_marker) continue;

        // Update the handle
        object.id = i;

        // Object is hidden; Ignore!
        if (!m3_object_visibility(object, true)) continue;

        // Get the root position of the object
        m3_pos_object root = m3_pos_object_get(object);

        // Transform camera position into cameraspace
        m3_pos_root_reverse_rlock(&root, camera->pos, camera->quat, object);

        // Get all segments that make up this object
        uint16_t length = m3_object_segment_length(object);
        m3_segment_t* buf = m3_object_segment_buf(object);

        // Live + Static positions of both current and previous vectors in cameraspace
        m3_pos_chain prev;
        m3_pos_chain pos = {
            root.loc,
            { 0, 0, 0 }
        };

        for (uint16_t j = 0; j < length; j++) {
            prev = pos;

            // Get position of next segment
            pos = m3_pos_segment_next(root, buf[j], prev.sta);

            // Get non-positional segment info
            uint8_t color = buf[j].color;

            // Only attempt to render if segment is visible
            if (color == M3_COLOR_INVISIBLE) continue;

            m3_bb bbox = m3_raster_bb(
                pos.live.x,
                pos.live.y,
                prev.live.x,
                prev.live.y
            );

            // Only rasterize if bounding box of vector falls within camera
            if (!m3_raster_bb_isect(bbox, camera_bb)) continue;

            int16_t x0 = (int16_t) roundf(prev.live.x * x_scale);
            int16_t y0 = (int16_t) roundf(prev.live.y * y_scale);
            int16_t x1 = (int16_t) roundf(pos.live.x * x_scale);
            int16_t y1 = (int16_t) roundf(pos.live.y * y_scale);

            // Mirror x0/x1 about the y-axis
            if (orientation & M3_ORIENTATION_HFLIP) {
                x0 = -x0;
                x1 = -x1;
            }

            // Mirror y0/y1 about the x-axis
            if (orientation & M3_ORIENTATION_VFLIP) {
                y0 = -y0;
                y1 = -y1;
            }

            // Transform desired coordinates to screenspace
            m3_raster_line(
                target,
                width,
                height, 
                x0 + screen_offset_x,
                y0 + screen_offset_y,
                x1 + screen_offset_x,
                y1 + screen_offset_y,
                color,
                orientation
            );
        }
    }

    return M3_SUCCESS;
}
