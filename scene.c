#include "./scene.h"

m3_scene_handle_t m3_scene_create_d(
    uint8_t seg_size,
    uint8_t obj_size
) {
    // Attempt to allocate space for segment vmem
    m3_segment_t* seg_buf = (m3_segment_t*) malloc(((int) seg_size) * sizeof(m3_segment_t));
    if (seg_buf == NULL) return NULL;

    // Attempt to allocate space for object vmem
    m3_object_t* obj_buf = (m3_object_t*) malloc(((int) seg_size) * sizeof(m3_object_t));
    if (obj_buf == NULL) {
        free(seg_buf);
        return NULL;
    }

    // Rely on static handler for actual struct creation
    m3_scene_handle_t scene = m3_scene_create_s(seg_buf, seg_size, obj_buf, obj_size);

    // Free now-unused resources
    if (scene == NULL) {
        free(seg_buf);
        free(obj_buf);
        return NULL;
    }

    return scene;
}

m3_scene_handle_t m3_scene_create_s(
    m3_segment_t* seg_buf,
    uint8_t seg_size,
    m3_object_t* obj_buf,
    uint8_t obj_size
) {
    
    // Attempt to allocate space for scene
    m3_scene_t* scene = (m3_scene_t*) malloc(sizeof(m3_scene_t));
    if (scene == NULL) return NULL;

    // Store segment/object buffers
    scene->seg_size = seg_size;
    scene->obj_size = obj_size;
    scene->seg_buf = seg_buf;
    scene->obj_buf = obj_buf;

    // Initialize all heap memory as empty
    memset(seg_buf, 0, ((int) seg_size) * sizeof(*seg_buf));
    memset(obj_buf, 0, ((int) obj_size) * sizeof(*obj_buf));

    // Successfully created scene!
    return scene;
}

/**
 * Return the index of some available segment slot. Runs in O(n) time
 * @param scene The scene to search within; Scene is assumed to exist.
 * @returns index on success, -1 on failure
 */
int16_t _m3_scene_segment_avail(m3_scene_handle_t scene) {
    uint16_t total = scene->seg_size == 0 ? 256 : scene->seg_size;

    for (uint16_t i = 0; i < total; i++) {
        if (!scene->seg_buf[i]._marker) {
            return (uint8_t) i;
        }
    }

    // Failed to find proper index
    return -1;
}

/**
 * Return the index of some available object slot. Runs in O(n) time
 * @param scene The scene to search within; Scene is assumed to exist.
 * @returns index on success, -1 on failure
 */
int16_t _m3_scene_object_avail(m3_scene_handle_t scene) {
    uint16_t total = scene->obj_size == 0 ? 256 : scene->obj_size;

    for (uint16_t i = 0; i < total; i++) {
        if (!scene->obj_buf[i]._marker) {
            return (uint8_t) i;
        }
    }

    // Failed to find proper index
    return -1;
}
