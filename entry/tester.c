// Used to test Micro3d library

#include "../micro3d.h"
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

    // Create camera
    m3_ocamera_handle_t camera = m3_ocamera_create();
    m3_ocamera_resize(camera, 32, 16);
    m3_ocamera_position(camera, (m3_vec){ 0, 0, 0 });

    // m3_vec dir = { 1, 0, 0 };
    // m3_vec up = { 0, 0, 1 };

    // m3_vec_normalize(&dir);
    // m3_vec_normalize(&up);
    // m3_quat quat = m3_vec_to_quat(dir, up);

    m3_quat quat = { 0, 0, 0, 1 };
    m3_quat_normalize(&quat);
    m3_ocamera_pivot(camera, quat);

    // Allocate space for camera to render to
    const int width = 32;
    const int height = 16;

    uint8_t target[width * height / 8];

    const int itts = 12;
    for (int i = 0; i <= itts; i++) {

        // Clear frame
        memset(target, 0, sizeof(target));

        m3_quat quat = { 0, 0, 127 * sin(i * 3.14159 / itts), 127 * cos(i * 3.14159 / itts) };
        m3_ocamera_pivot(camera, quat);

        // Render 
        m3_ocamera_render(camera, scene, target, width, height);

        // Print out rasterized image
        printf("\n");
        for (int y = height - 1; y >= 0; y--) {
            printf("| ");
            for (int x = 0; x < width; x++) {
                uint16_t index = (y*width + x) / 8;
                uint8_t row = target[index];

                if (x == width / 2 && y == height / 2) {
                    printf("::");
                    continue;
                }
                
                if (row & (0x01 << (x % 8)))
                    printf("[]");
                else printf("  ");
            }
            printf(" |\n");
        }
        printf("\n");

        if (i != itts) usleep(500000);
    }
    return 0;
}
