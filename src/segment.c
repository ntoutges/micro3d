#include "./segment.h"

m3_segment_t* _m3_segment_deref(m3_segment_handle_t handle); // Get a pointer to the underlying segment from a segment handle; Assumes a valid handle

/**
 * Create an new instance of an empty segment
 * @return The empty segment data
 */
m3_segment_t _m3_segment_new() {
    return (m3_segment_t) {
        .x = 0,
        .y = 0,
        .z = 0,
        .visible = 1,
        .absolute = 0,
        ._marker = 1
    };
}

bool m3_segment_exists(m3_segment_handle_t segment) {
    return segment.owner.owner != NULL;
}

m3_err_t m3_segment_offset(
    m3_segment_handle_t handle,
    m3_vec vec
) {
    if (handle.owner.owner == NULL) return M3_ERR_EXIST_A; // Segment doesn't exist

    // Ensure vec bounds are valid (Fit into 4-bit int fields)
    if (
        vec.x < -8 || vec.x > 7
        || vec.y < -8 || vec.y > 7
        || vec.z < -8 || vec.z > 7
    ) return M3_ERR_POS;

    // Update bounds of segment
    m3_segment_t* segment = _m3_segment_deref(handle);
    segment->x = 0x0F & vec.x;
    segment->y = 0x0F & vec.y;
    segment->z = 0x0F & vec.z;

    // Success!
    return M3_SUCCESS;
}

m3_err_t m3_segment_visible(m3_segment_handle_t handle, bool visible) {
    if (handle.owner.owner == NULL) return M3_ERR_EXIST_A; // Segment doesn't exist

    // Update visibility of segment
    m3_segment_t* segment = _m3_segment_deref(handle);
    segment->visible = visible ? 1 : 0;

    printf("(%d) visible: %d\n", handle.id, segment->visible);

    // Success!
    return M3_SUCCESS;
}

m3_err_t m3_segment_absolute(
    m3_segment_handle_t handle,
    bool absolute
) {
    if (handle.owner.owner == NULL) return M3_ERR_EXIST_A; // Segment doesn't exist

    // Update visibility of segment
    m3_segment_t* segment = _m3_segment_deref(handle);
    segment->absolute = absolute ? 1 : 0;

    // Success!
    return M3_SUCCESS;
}

bool m3_segment_visibility(m3_segment_handle_t handle) {
    if (handle.owner.owner == NULL) return false;
    return _m3_segment_deref(handle)->visible;
}

m3_object_handle_t m3_segment_owner(m3_segment_handle_t segment) {
    return segment.owner;
}

inline m3_segment_t* _m3_segment_deref(m3_segment_handle_t handle) {
    m3_scene_handle_t scene = handle.owner.owner;
    m3_object_t* object = &(scene->obj_buf[handle.owner.id]);

    return &(object->segments[handle.id]);
}