#include "./ocamera.h"

m3_ocamera_handle_t m3_ocamera_create() {
    m3_ocamera_t* camera = (m3_ocamera_t*) malloc(sizeof(m3_ocamera_t));

    if (!camera) return NULL;

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
    uint8_t height
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

    // printf("C: (%d, %d):(%d,%d)\n", camera_bb.x_min, camera_bb.y_min, camera_bb.x_max, camera_bb.y_max);

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
        m3_pos_root_reverse(&root, camera->pos, camera->quat);

        // Get all segments that make up this object
        uint16_t length = m3_object_segment_length(object);
        uint8_t* buf = m3_object_segment_buf(object);

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
            m3_segment_handle_t segment = m3_segment_get(scene, buf[j]);
            bool visible = m3_segment_visibility(segment);

            // Only attempt to render if segment is visible
            if (!visible) continue;

            m3_bb bbox = m3_raster_bb(
                pos.live.x,
                pos.live.y,
                prev.live.x,
                prev.live.y
            );

            // Only rasterize if bounding box of vector falls within camera
            if (!m3_raster_bb_isect(bbox, camera_bb)) continue;

            // printf("%d: (%d, %d):(%d,%d)\n",
            //     j, 
            //     (int16_t) roundf(prev.live.x * x_scale) + screen_offset_x,
            //     (int16_t) roundf(prev.live.y * y_scale) + screen_offset_y,
            //     (int16_t) roundf(pos.live.x * x_scale) + screen_offset_x,
            //     (int16_t) roundf(pos.live.y * y_scale) + screen_offset_y
            // );

            // Transform desired coordinates to screenspace
            m3_raster_line(
                target,
                width,
                height, 
                (int16_t) roundf(prev.live.x * x_scale) + screen_offset_x,
                (int16_t) roundf(prev.live.y * y_scale) + screen_offset_y,
                (int16_t) roundf(pos.live.x * x_scale) + screen_offset_x,
                (int16_t) roundf(pos.live.y * y_scale) + screen_offset_y
            );
        }
    }

    return M3_SUCCESS;
}
