#include "./object.h"

extern int16_t _m3_scene_object_avail(m3_scene_handle_t);

bool _m3_object_update_segments(m3_object_t* object, uint16_t size);
uint16_t _m3_object_segments_get_size(m3_object_t* object);
uint16_t _m3_object_segments_get_capacity(m3_object_t* object);
void _m3_object_segments_set_size(m3_object_t* object, uint16_t size);
void _m3_object_segments_set_capacity(m3_object_t* object, uint16_t cap);


m3_object_handle_t m3_object_create(m3_scene_handle_t scene) {
    if (scene == NULL) return ((m3_object_handle_t) { NULL, 0 });

    // Attempt to find open slot for object
    int16_t index = _m3_scene_object_avail(scene);
    if (index == -1) return ((m3_object_handle_t) { NULL, 0 });

    m3_object_t* object = &(scene->obj_buf[index]);

    // Mark as used
    object->_marker = 1;

    // Mark object as having no parent
    object->parent = index;

    // Populate object position
    object->pos = (m3_vec){ 0, 0, 0 };

    // Populate object orientation
    object->quat = (m3_quat){ 0, 0, 0, 127 };

    // By default: Mark object as visible
    object->visible = 1;

    // By default: no axes locked
    object->lx = 0;
    object->ly = 0;

    // Initialize arraylist holding segments
    object->scap = 0b111; // Special case -> capacity = 0
    object->soffset = 0; // 0 - 0 = size
    object->smem = 0; // 0 = dynamic allocation
    object->segments = NULL;

    return ((m3_object_handle_t) { scene, index });
}

void m3_object_destroy(m3_object_handle_t* object) {
    if (object->owner == NULL) return; // Cannot destroy that which does not exist

    // Get object from object vmem
    m3_object_t* obj = &(object->owner->obj_buf[object->id]);

    // Mark object as unused in object vmem
    obj->_marker = 0;

    // Free up object resources
    free(obj->segments);

    // Mark object as non-existant
    object->owner = NULL;
}

bool m3_object_exists(m3_object_handle_t object) {
    return object.owner != NULL;
}

m3_scene_handle_t m3_object_owner(m3_object_handle_t object) {
    return object.owner;
}

m3_err_t m3_object_alloc_s(
    m3_object_handle_t object,
    uint8_t* buf,
    uint8_t len
) {
    if (!object.owner) return M3_ERR_EXIST_A;

    // Get object
    m3_object_t* obj = &(object.owner->obj_buf[object.id]);

    // Get current size of segments list
    uint16_t size = _m3_object_segments_get_size(obj);

    // Update size/capacity to match new static buffer
    _m3_object_segments_set_capacity(obj, len);

    // Get the stored length 2 (* floor(log2(len))
    uint16_t st_len = _m3_object_segments_get_capacity(obj);

    // Copy as many segments as possible from object's segment list into `buf`
    uint16_t copy_ct = size < st_len ? size : st_len;
    memcpy(buf, obj->segments, copy_ct * sizeof(uint8_t));

    // Update size of segments
    _m3_object_segments_set_size(obj, copy_ct);

    // Free old buffer if required
    if (!obj->smem) {
        free(obj->segments);
    }

    // Update segments to static buffer
    obj->segments = buf;
    obj->smem = 1;

    return M3_SUCCESS;
}

m3_err_t m3_object_free_s(
    m3_object_handle_t object
) {
    if (!object.owner) return M3_ERR_EXIST_A;

    // Get object
    m3_object_t* obj = &(object.owner->obj_buf[object.id]);
    if (!obj->smem) return M3_ERR_EXIST_B;

    // Get current size of segments list
    uint16_t size = _m3_object_segments_get_size(obj);

    // Need to reallocate
    if (size != 0) {
        uint8_t* segments = malloc(sizeof(uint8_t) * size);

        // Unable to allocate space for segments
        if (!segments) return M3_ERR_ALLOC;

        // Transfer all segments references to `segments`
        memcpy(segments, obj->segments, sizeof(uint8_t) * size);

        // Update pointer
        obj->segments = segments;

    }

    // Update pointer to null, as nothing needs to be allocated
    else {
        obj->segments = NULL;
    }

    // Update size/capacity fields
    _m3_object_segments_set_capacity(obj, size);
    _m3_object_segments_set_size(obj, size);

    return M3_SUCCESS;
}

m3_object_ires_t m3_object_push_segment(
    m3_object_handle_t object,
    m3_segment_handle_t segment
) {
    if (!object.owner) return ((m3_object_ires_t) { 0, M3_ERR_EXIST_A });
    if (segment.owner != object.owner) return ((m3_object_ires_t) { 0, M3_ERR_EXIST_B });

    // Get object
    m3_object_t* obj = &(object.owner->obj_buf[object.id]);

    // Attempt to increase size of segments list
    uint16_t size = _m3_object_segments_get_size(obj);
    bool expanded = _m3_object_update_segments(
        obj,
        size + 1
    );
    if (!expanded) return ((m3_object_ires_t) { 0, M3_ERR_ALLOC });

    // Insert segment into segments list
    obj->segments[size] = segment.id;

    return ((m3_object_ires_t) { size, M3_SUCCESS });
}

m3_object_ires_t m3_object_replace_segment(
    m3_object_handle_t object,
    m3_segment_handle_t segment,
    uint8_t index
) {
    if (!object.owner) return ((m3_object_ires_t) { 0, M3_ERR_EXIST_A });
    if (segment.owner != object.owner) return ((m3_object_ires_t) { 0, M3_ERR_EXIST_B });

    // Get object
    m3_object_t* obj = &(object.owner->obj_buf[object.id]);

    // Perform bounds check
    uint16_t size = _m3_object_segments_get_size(obj);
    if (index >= size) return ((m3_object_ires_t) { 0, M3_ERR_BOUNDS });

    // Perform replacement
    obj->segments[index] = segment.id;
    
    return ((m3_object_ires_t) { index, M3_SUCCESS });
}

m3_object_ires_t m3_object_insert_segment(
    m3_object_handle_t object,
    m3_segment_handle_t segment,
    uint8_t index
) {
    if (!object.owner) return ((m3_object_ires_t) { 0, M3_ERR_EXIST_A });
    if (segment.owner != object.owner) return ((m3_object_ires_t) { 0, M3_ERR_EXIST_B });

    // Get object
    m3_object_t* obj = &(object.owner->obj_buf[object.id]);

    // Perform bounds check
    uint16_t size = _m3_object_segments_get_size(obj);
    if (index >= size) return ((m3_object_ires_t) { 0, M3_ERR_BOUNDS });

    // Attempt to increase size of segments list
    bool expanded = _m3_object_update_segments(
        obj,
        size + 1
    );
    if (!expanded) return ((m3_object_ires_t) { 0, M3_ERR_ALLOC });

    // Ripple move all elements to the right by 1 place
    memmove(obj->segments + index + 1, obj->segments + index, (size - index) * sizeof(uint8_t));

    // Insert element into vacated location at `index`
    obj->segments[index] = segment.id;

    return ((m3_object_ires_t) { index, M3_SUCCESS });
}

m3_err_t m3_object_remove_segment(
    m3_object_handle_t object,
    uint8_t index
) {
    if (!object.owner) return M3_ERR_EXIST_A;

    // Get object
    m3_object_t* obj = &(object.owner->obj_buf[object.id]);

    // Perform bounds check
    uint16_t size = _m3_object_segments_get_size(obj);
    if (index >= size) return M3_ERR_BOUNDS;

    // Ripple move all elements to the left by 1 place
    memmove(obj->segments + index, obj->segments + index + 1, (size - index) * sizeof(uint8_t));

    // Shrink desired size
    _m3_object_update_segments(obj, size - 1);

    return M3_SUCCESS;
}

m3_err_t m3_object_clear(m3_object_handle_t object) {
    if (!object.owner) return M3_ERR_EXIST_A;

    // Get object
    m3_object_t* obj = &(object.owner->obj_buf[object.id]);

    // Free any allocated array
    free(obj->segments);

    // Reset arraylist holding segments
    obj->scap = 0b111; // Special case -> capacity = 0
    obj->soffset = 0; // 0 - 0 = size
    obj->segments = NULL;

    return M3_SUCCESS;
}

/**
 * Update the size/capacity of the given object's segments array to match the desired size
 * @param object    The object to update
 * @param size      The size to grow/shrink the capacity to fit
 * @returns         `true` if the capacity change worked, `false` otherwise
 */
bool _m3_object_update_segments(m3_object_t* object, uint16_t size) {
    if (size > 256) return false; // Unable to expand size further

    // Get current list values
    uint16_t curr_cap = _m3_object_segments_get_capacity(object);

    uint16_t new_cap = curr_cap;
    if (size > curr_cap) new_cap = new_cap ? new_cap * 2 : 1; // Need to expand capacity
    else if (size < curr_cap / 4) new_cap /= 2; // Need to shrink capacity

    // Need to update capacity; Only allowed to if _not_ static memory
    if (new_cap != curr_cap && !object->smem) {

        // Attempt to allocate space for the larger/smaller segment list
        uint8_t* segments = (uint8_t*) realloc(object->segments, new_cap * sizeof(uint8_t));
        if (segments == NULL) return false;

        // Update pointer to new memory region
        object->segments = segments;

        // Update capacity and size
        _m3_object_segments_set_capacity(object, new_cap);
        _m3_object_segments_set_size(object, size);
        
        // Success!
        return true;
    }

    // Check if capacity grow requested while static
    if (object->smem && new_cap > curr_cap) return false; // Unable to expand static memory

    // Simply need to update size value
    _m3_object_segments_set_size(object, size);
    return true;
}

/**
 * Get the size of the segments list in an object
 * @param object    The object to get the size of
 * @returns         The computed size of the segments list in the range [0, 256]
 */
uint16_t _m3_object_segments_get_size(m3_object_t* object) {
    uint16_t capacity = _m3_object_segments_get_capacity(object);
    return capacity - object->soffset; // size = capacity - soffset
}

/**
 * Get the capacity of the segments list in an object
 * @param object    The object to get the capacity of
 * @returns         The computed capacity of the segments list in the range [0, 256]
 */
uint16_t _m3_object_segments_get_capacity(m3_object_t* object) {
    return object->scap == 0b111
        ? 0 // Special case: capacity = 0
        : ((uint16_t) 0x0001) << object->scap; // capacity = 2 ** scap
}

/**
 * Set the size of the segments list in an object
 * Assumes size <= capacity
 * @param object    The object to set the size of
 * @param size      The size to set
 */
void _m3_object_segments_set_size(m3_object_t* object, uint16_t size) {
    uint16_t capacity = _m3_object_segments_get_capacity(object);
    object->soffset = capacity - size;
}

/**
 * Set the capacity of the segments list in an object
 * Assumes size = 2 ** n; Note that this does _NOT_ update the size `soffset` value
 * @param object    The object to set the capacity of
 * @param cap       The capacity to set
 */
void _m3_object_segments_set_capacity(m3_object_t* object, uint16_t cap) {
    // Special case: scap = 0
    if (cap == 0) {
        object->scap = 0b111;
        return;
    }

    // Perform floor(log(size))
    uint8_t scap = 0;
    while (cap >>= 1) scap++;

    // Store scap = floor(log(size))
    object->scap = scap;

    // Restore size
    cap = 0x0001 << scap;
}

m3_err_t m3_object_pset(m3_object_handle_t object, m3_object_handle_t parent) {
    if (!object.owner) return M3_ERR_EXIST_A;
    if (parent.owner != object.owner) return M3_ERR_EXIST_B;

    m3_object_t* objects = object.owner->obj_buf;

    // Get object
    m3_object_t* obj = &(objects[object.id]);
    
    // Walk up parent tree to ensure `object` isn't already in the hierarchy
    uint8_t head = parent.id;
    while (objects[head].parent != head) {
        head = objects[head].parent;

        // Circular hierarchy detected!
        if (head == object.id) return M3_ERR_CIRCULAR;
    }

    // No potential circular hierarchies detected; Carry on!
    obj->parent = parent.id;
    return M3_SUCCESS;
}

m3_err_t m3_object_pclear(m3_object_handle_t object) {
    if (!object.owner) return M3_ERR_EXIST_A;

    // Get object
    m3_object_t* obj = &(object.owner->obj_buf[object.id]);
    
    // Reset parent to self to indicate no parent
    obj->parent = object.id;

    return M3_SUCCESS;
}

uint16_t m3_object_segment_length(m3_object_handle_t object) {
    if (object.owner == NULL) return 0;

    // Get object
    m3_object_t* obj = &(object.owner->obj_buf[object.id]);

    return _m3_object_segments_get_size(obj);
}

uint8_t* m3_object_segment_buf(m3_object_handle_t object) {
    if (object.owner == NULL) return NULL;

    // Get object
    m3_object_t* obj = &(object.owner->obj_buf[object.id]);

    return obj->segments;
}

m3_segment_handle_t m3_object_segment_get(m3_object_handle_t object, uint8_t index) {
    if (!object.owner) return (m3_segment_handle_t){ NULL, 0 };

    // Get object
    m3_object_t* obj = &(object.owner->obj_buf[object.id]);

    uint16_t size = _m3_object_segments_get_size(obj);
    if (index >= size) return (m3_segment_handle_t){ NULL, 0 }; // Invalid segment

    return (m3_segment_handle_t){
        object.owner,
        obj->segments[index]
    };
}

m3_err_t m3_object_position(m3_object_handle_t object, m3_vec vec) {
    if (!object.owner) return M3_ERR_EXIST_A;

    // Get object
    m3_object_t* obj = &(object.owner->obj_buf[object.id]);

    // Update internal vector
    obj->pos = vec;

    return M3_SUCCESS;
}

m3_err_t m3_object_pivot(m3_object_handle_t object, m3_quat quat) {
    if (!object.owner) return M3_ERR_EXIST_A;

    // Get object
    m3_object_t* obj = &(object.owner->obj_buf[object.id]);

    // Update internal quaternion
    obj->quat = quat;

    return M3_SUCCESS;
}

m3_err_t m3_object_visible(m3_object_handle_t object, bool visible) {
    if (!object.owner) return M3_ERR_EXIST_A;

    // Get object
    m3_object_t* obj = &(object.owner->obj_buf[object.id]);

    // Update visibility state
    obj->visible = visible ? 1 : 0;

    return M3_SUCCESS;
}

m3_err_t m3_object_rlock(
    m3_object_handle_t object,
    uint8_t rlock
) {
    if (!object.owner) return M3_ERR_EXIST_A;

    // Get object
    m3_object_t* obj = &(object.owner->obj_buf[object.id]);

    // Update rotation lock state
    obj->lx = (rlock & M3_RLOCK_X) ? 1 : 0;
    obj->ly = (rlock & M3_RLOCK_Y) ? 1 : 0;

    // Success!
    return M3_SUCCESS;
}

bool m3_object_visibility(m3_object_handle_t object, bool chain) {
    if (!object.owner) return false;

    uint8_t head_id = object.id;
    m3_object_t* head_obj = &(object.owner->obj_buf[object.id]);

    bool visible = head_obj->visible;

    // Walk up chain if required
    if (chain && visible) {
        while (head_id != head_obj->parent) {
            // Walk up the parent chain
            head_id = head_obj->parent;
            head_obj = &object.owner->obj_buf[head_id];

            // Encountered some invisible parent object: We are _also_ hidden!
            if (!head_obj->visible) {
                visible = false;
                break;
            }
        }
    }

    return visible;
}
