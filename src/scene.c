#include "./scene.h"

m3_scene_handle_t m3_scene_create_d(
    uint8_t obj_size
) {
    // Attempt to allocate space for object vmem
    m3_object_t* obj_buf = (m3_object_t*) malloc(((int) obj_size) * sizeof(m3_object_t));
    if (obj_buf == NULL) {
        return NULL;
    }

    // Attempt to allocate space for scene
    m3_scene_t* scene = (m3_scene_t*) malloc(sizeof(m3_scene_t));
    if (scene == NULL) {
        free(obj_buf);
        return NULL;
    }

    // Rely on static handler for actual struct instantiation
    m3_scene_create_s(scene, obj_buf, obj_size);

    return scene;
}

m3_scene_handle_t m3_scene_create_s(
    m3_scene_t* scene,
    m3_object_t* obj_buf,
    uint8_t obj_size
) {
    // Store segment/object buffers
    scene->obj_size = obj_size;
    scene->obj_buf = obj_buf;

    // Initialize all heap memory as empty
    memset(obj_buf, 0, ((int) obj_size) * sizeof(*obj_buf));

    // Successfully created scene!
    return scene;
}

m3_object_handle_t m3_scene_object_get(
    m3_scene_t* scene,
    uint8_t id
) {
    return (m3_object_handle_t) {
        .owner = scene,
        .id = id
    };
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
