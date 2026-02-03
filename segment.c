#include "./segment.h"

extern int16_t _m3_scene_segment_avail(m3_scene_handle_t);

m3_segment_handle_t m3_segment_create(m3_scene_handle_t scene) {
    // No scene; Failed to create segment
    if (scene == NULL) return ((m3_segment_handle_t) { NULL, 0 });

    // Attempt to find open slot for segment
    int16_t index = _m3_scene_segment_avail(scene);
    if (index == -1) return ((m3_segment_handle_t) { NULL, 0 });

    m3_segment_t* segment = &(scene->seg_buf[index]);
    
    // Mark as used
    segment->_marker = 1;

    // Populate segment data
    segment->x = 0;
    segment->y = 0;
    segment->z = 0;

    segment->visible = 1;
    segment->absolute = 0;

    return ((m3_segment_handle_t) { scene, index });
}

void m3_segment_destroy(m3_segment_handle_t* segment) {
    if (segment->owner == NULL) return; // Cannot destroy that which does not exist

    // Mark segment as unused in segment vmem
    segment->owner->seg_buf[segment->id]._marker = 0;
    
    // Mark segment as non-existant
    segment->owner = NULL;
}

bool m3_segment_exists(m3_segment_handle_t segment) {
    return segment.owner != NULL;
}

m3_scene_handle_t m3_segment_owner(m3_segment_handle_t segment) {
    return segment.owner;
}

inline m3_segment_handle_t m3_segment_get(m3_scene_handle_t owner, uint8_t id) {
    return (m3_segment_handle_t) {
        owner, id
    };
}

m3_err_t m3_segment_offset(m3_segment_handle_t handle, m3_vec vec) {
    if (handle.owner == NULL) return M3_ERR_EXIST_A; // Segment doesn't exist

    // Ensure vec bounds are valid (Fit into 4-bit int fields)
    if (
        vec.x < -8 || vec.x > 7
        || vec.y < -8 || vec.y > 7
        || vec.z < -8 || vec.z > 7
    ) return M3_ERR_POS;

    // Update bounds of segment
    m3_segment_t* segment = &(handle.owner->seg_buf[handle.id]);
    segment->x = 0x0F & vec.x;
    segment->y = 0x0F & vec.y;
    segment->z = 0x0F & vec.z;

    // Success!
    return M3_SUCCESS;
}

m3_err_t m3_segment_visible(m3_segment_handle_t handle, bool visible) {
    if (handle.owner == NULL) return M3_ERR_EXIST_A; // Segment doesn't exist

    // Update visibility of segment
    m3_segment_t* segment = &(handle.owner->seg_buf[handle.id]);
    segment->visible = visible ? 1 : 0;

    printf("(%d) visible: %d\n", handle.id, segment->visible);

    // Success!
    return M3_SUCCESS;
}

m3_err_t m3_segment_absolute(
    m3_segment_handle_t handle,
    bool absolute
) {
    if (handle.owner == NULL) return M3_ERR_EXIST_A; // Segment doesn't exist

    // Update visibility of segment
    m3_segment_t* segment = &(handle.owner->seg_buf[handle.id]);
    segment->absolute = absolute ? 1 : 0;

    // Success!
    return M3_SUCCESS;
}

bool m3_segment_visibility(m3_segment_handle_t handle) {
    if (handle.owner == NULL) return false;
    return handle.owner->seg_buf[handle.id].visible;
}
