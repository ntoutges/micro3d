// Used to test Micro3d library

#include "./micro3d.h"
#include "stdio.h"

int main() {
    // Create a test scene
    m3_scene_handle_t scene = m3_scene_create_d(128, 16);

    // Populate scene with a 1x1 box
    
    // Create base segments
    m3_segment_handle_t vertical = m3_segment_create(scene);
    m3_segment_handle_t horizontal = m3_segment_create(scene);
    m3_segment_handle_t rts = m3_segment_create(scene);

    // Populate base segments
    m3_segment_offset(vertical, (m3_vec) { 0, 0, 1 });
    m3_segment_offset(horizontal, (m3_vec) { 1, 0, 0 });
    m3_segment_offset(rts, (m3_vec) { -1, 0, -1 });
    m3_segment_visible(rts, false);

    // Create object to use segments
    m3_object_handle_t box = m3_object_create(scene);
    
    // Fill object with segments
    m3_object_push_segment(box, vertical);   // |
    m3_object_push_segment(box, horizontal); // |^
    m3_object_push_segment(box, rts);        // |^
    m3_object_push_segment(box, horizontal); // |#
    m3_object_push_segment(box, vertical);   // |#|

    // TODO: Create render pipeline

    // Required: transformed object position + quat getter
    // Required: fn to transform coordinates given quat

    return 0;
}