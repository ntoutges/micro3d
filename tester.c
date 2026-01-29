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
    m3_segment_offset(vertical, (m3_vec) { 0, 2, 0 });
    m3_segment_offset(horizontal, (m3_vec) { 2, 0, 0 });
    m3_segment_offset(rts, (m3_vec) { -2, -2, 0 });
    m3_segment_visible(rts, false);

    // Create object to use segments
    m3_object_handle_t box = m3_object_create(scene);
    m3_object_position(box, (m3_vec) { 1, 1, 1 });

    // Create parent object
    m3_object_handle_t parent = m3_object_create(scene);
    m3_object_position(parent, (m3_vec) { 0, 0, 1 });
    m3_object_pset(box, parent);
    
    // Fill object with segments
    m3_object_push_segment(box, vertical);   // |
    m3_object_push_segment(box, horizontal); // |^
    m3_object_push_segment(box, rts);        // |^
    m3_object_push_segment(box, horizontal); // |#
    m3_object_push_segment(box, vertical);   // |#|

    // Rotate box by 45 degrees
    m3_vec dir = { 1, 1, 0 };
    m3_vec up = { 0, 0, 1 };
    m3_vec_normalize(&dir);
    m3_quat rot = m3_vec_to_quat(dir, up);
    m3_object_pivot(box, rot);

    // DEBUG: Print out box data
    m3_pos_object root = m3_pos_object_get(box);

    printf(
        "Box: (%d, %d, %d), (%.1f, %.1f, %.1f, %.1f)\n",
        root.loc.x, root.loc.y, root.loc.z,
        root.rot.x / 127.0, root.rot.y / 127.0, root.rot.z / 127.0, root.rot.w / 127.0
    );

    // DEBUG: Loop through all box segments and print out their positions
    uint16_t length = m3_object_segment_length(box);
    uint8_t* buf = m3_object_segment_buf(box);
    m3_pos_chain pos = {
        { 0, 0, 0 },
        { 0, 0, 0 }
    };
    for (uint16_t i = 0; i < length; i++) {
        pos = m3_pos_segment_next(root, buf[i], pos.sta);

        printf(
            "Segment[%d] (%d): (%d, %d, %d)\n",
            i, buf[i],
            pos.live.x, pos.live.y, pos.live.z
        );
    }

    // TODO: Create render pipeline

    // Required: transformed object position + quat getter
    // Required: fn to transform coordinates given quat

    return 0;
}