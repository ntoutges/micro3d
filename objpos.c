#include "objpos.h"

m3_pos_object m3_pos_object_get(m3_object_handle_t object) {
    if (object.owner == NULL) return (m3_pos_object){ { 0, 0, 0 }, { 0, 0, 0, 127 } }; // Object doesn't exist

    // Get the raw object instance
    m3_object_t* obj = &object.owner->obj_buf[object.id];

    // Extract the base position
    m3_pos_object pos = (m3_pos_object){ obj->pos, obj->quat, object.owner };

    // Apply parent transformation to base position, if required
    uint8_t head_id = object.id;
    m3_object_t* head_obj = obj;
    while (head_id != head_obj->parent) {
        // Walk up the parent chain
        head_id = head_obj->parent;
        head_obj = &object.owner->obj_buf[head_id];

        // Apply parent quat + vec rotation
        m3_quat_rotate(&pos.rot, head_obj->quat);
        m3_vec_rotate(&pos.loc, head_obj->quat);

        // Add parent pos to base vec
        m3_vec_add(&pos.loc, head_obj->pos);
    }

    // Object position has now been obtained!
    return pos;
}

// Notice that this is basically an alias to provide a default parameter
inline m3_pos_chain m3_pos_segment_get(m3_pos_object root, uint8_t segment) {
    return m3_pos_segment_next(root, segment, root.loc );
}

m3_pos_chain m3_pos_segment_next(m3_pos_object root, uint8_t segment, m3_vec prev) {
    if (root.owner == NULL) return (m3_pos_chain){ { 0, 0, 0 }, prev }; // Segment doesn't exist

    // Get the raw segment object
    m3_segment_t seg = root.owner->seg_buf[segment];

    // Extract segment position
    m3_vec s_pos = { seg.x, seg.y, seg.z };

    // Add `prev` position to segment for proper clean offset
    m3_vec_add(&s_pos, prev);

    // Clone static pos to create `live` position
    // Allows `s_pos` to represent clean object-space coordinates
    m3_vec l_pos = s_pos;

    // Update segment position from root
    m3_vec_rotate(&l_pos, root.rot);

    m3_vec_add(&l_pos, root.loc);

    return (m3_pos_chain){
        l_pos,
        s_pos
    };
}

void m3_pos_root_reverse(m3_pos_object* root, m3_vec pos, m3_quat quat) {
    if (root->owner == NULL) return; // Invalid root object

    // Find the inverse of the given quat
    // Only need the conjugate, as `quat` is garunteed normalized
    m3_quat inv_quat = m3_quat_conj(quat);
    
    // Unrotate root position by `quat`
    m3_quat_rotate(&(root->rot), inv_quat);

    // Calculate rotation offset
    m3_vec offset;
    m3_vec_sub(&offset, pos);

    // Untranslate root position by `pos`
    m3_vec_sub(&(root->loc), pos);
    m3_vec_rotate(&(root->loc), inv_quat);
}
