// Used to test Micro3d library

#include "./micro3d.h"
#include "stdio.h"

#include <unistd.h>

int main() {
    // Create a test scene
    m3_scene_handle_t scene = m3_scene_create_d(128, 16);

    // Populate scene with a 1x1 box
    
    // Create base segments
    m3_segment_handle_t vertical = m3_segment_create(scene);
    m3_segment_handle_t horizontal = m3_segment_create(scene);
    m3_segment_handle_t rts = m3_segment_create(scene);

    // Populate base segments
    m3_segment_offset(vertical, (m3_vec) { 0, 7, 0 });
    m3_segment_offset(horizontal, (m3_vec) { 7, 0, 0 });
    m3_segment_offset(rts, (m3_vec) { -7, -7, 0 });
    // m3_segment_visible(rts, false);

    // Create object to use segments
    m3_object_handle_t box = m3_object_create(scene);
    m3_object_position(box, (m3_vec) { 0, 0, 0 });

    // Fill object with segments
    m3_object_push_segment(box, vertical);   // |
    m3_object_push_segment(box, horizontal); // |^
    m3_object_push_segment(box, rts);        // |^
    m3_object_push_segment(box, horizontal); // |#
    m3_object_push_segment(box, vertical);   // |#|

    // RASTERIZATION!

    const int width = 32;
    const int height = 32;

    const int frames = 36; // Frames per animation
    const int start = 0*frames;
    const int end = 3*frames;

    // Store rotations
    m3_quat quats[3*frames + 1];
    quats[3*frames] = (m3_quat){ 0, 0, 0, 127 };

    // Rotate about x axis
    for (int t = 0; t < frames; t++) {
        // Create vector to rotate about
        m3_vec dir = { 1, 0, 0 };
        m3_vec up = {
            sin(t * 2 * 3.14159 / frames) * 127,
            0,
            cos(t * 2 * 3.14159 / frames) * 127
        };

        m3_vec_normalize(&dir);
        m3_vec_normalize(&up);
        quats[t] = m3_vec_to_quat(dir, up);
    }

    // Rotate about y axis
    for (int t = 0; t < frames; t++) {
        // Create vector to rotate about
        m3_vec dir = {
            cos(t * 2 * 3.14159 / frames) * 127,
            0,
            -sin(t * 2 * 3.14159 / frames) * 127,
        };
        m3_vec up = {
            sin(t * 2 * 3.14159 / frames) * 127,
            0,
            cos(t * 2 * 3.14159 / frames) * 127,
        };

        m3_vec_normalize(&dir);
        m3_vec_normalize(&up);
        quats[t + 1*frames] = m3_vec_to_quat(dir, up);

        // quats[t + 1*frames] = (m3_quat){
        //     0,
        //     sin(t * 2 * 3.14159 / frames) * 127,
        //     0,
        //     cos(t * 2 * 3.14159 / frames) * 127
        // };
    }

    // Rotate about z axis
    for (int t = 0; t < frames; t++) {
        // Create vector to rotate about
        m3_vec dir = {
            cos(t * 2 * 3.14159 / frames) * 127,
            sin(t * 2 * 3.14159 / frames) * 127,
            0
        };
        m3_vec up = { 0, 0, 1 };

        m3_vec_normalize(&dir);
        m3_vec_normalize(&up);
        quats[t + 2*frames] = m3_vec_to_quat(dir, up);
    }

    uint8_t target[width * height / 8] = {};

    // Fun: Rotate box by a small amount
    for (int t = start; t <= end; t++) {

        // Clear render target
        for (int i = 0; i < sizeof(target); i++) {
            target[i] = 0;
        }

        m3_quat quat = quats[t];
        printf("(%.2f, %.2f, %.2f, %.2f)\n", quat.x / 127.0, quat.y / 127.0, quat.z / 127.0, quat.w / 127.0);

        // Rotate box by quat
        m3_object_pivot(box, quat);

        // Update object root given the new rotation
        m3_pos_object root = m3_pos_object_get(box);

        uint16_t length = m3_object_segment_length(box);
        uint8_t* buf = m3_object_segment_buf(box);
        m3_pos_chain pos = {
            { 0, 0, 0 },
            { 0, 0, 0 }
        };
        m3_pos_chain prev = {
            { 0, 0, 0 },
            { 0, 0, 0 }
        };
        for (uint16_t i = 0; i < length; i++) {
            pos = m3_pos_segment_next(root, buf[i], prev.sta);

            m3_segment_handle_t segment = m3_segment_get(m3_object_owner(box), buf[i]);

            // Don't render if invisible
            if (m3_segment_visibility(segment)) {

                // Render a line projected onto the z-plane
                m3_raster_line(
                    target,
                    width,
                    height,
                    prev.live.x + width / 2,
                    prev.live.y + height / 2,
                    pos.live.x + width / 2,
                    pos.live.y + height / 2
                );
            }

            prev = pos;
        }

        // Print out rasterized image
        printf("\n");
        for (int y = height - 1; y >= 0; y--) {
            if (y == height - 1) printf("%2d", t);
            else printf("| ");
            for (int x = 0; x < height; x++) {
                uint8_t row = target[(y*height + x) / 8];

                if (x == height / 2 && y == height / 2) {
                    printf("::");
                    continue;
                }
                
                if (row & (0x01 << x % 8))
                    printf("||");
                else printf("  ");
            }

            printf(" |\n");
        }
        printf("\n");

        if (t != end) usleep(5 * 1000000 / (end - start + 1));
    }

    // TODO: Create render pipeline

    // Required: transformed object position + quat getter
    // Required: fn to transform coordinates given quat

    return 0;
}