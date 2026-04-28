// Used to test Micro3d library

#include "../src/micro3d.h"
#include "stdio.h"

#include <unistd.h>

m3_scene_t scene;
m3_ocamera_t camera;

m3_object_handle_t geo;

m3_object_t scene_objects[3];

m3_segment_t geo_segments[16];

int main() {
    // --- Populate scene with some geometry ---

    // Create the main scene
    m3_scene_create_s(
        &scene,
        scene_objects,
        sizeof(scene_objects) / sizeof(*scene_objects)
    );

    // Create camera
    m3_ocamera_create_s(&camera);
    m3_ocamera_resize(&camera, 16, 8);
    m3_ocamera_position(&camera, (m3_vec){ 2, 2, 2 });

    // Create required objects
    geo = m3_object_create(&scene);

    // Assign objects to their buffers
    m3_object_alloc_s(geo, geo_segments, sizeof(geo_segments));

    // -- Add in geometry --
    m3_segment_handle_t s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, sm0, sm1, sm2;

    m3_object_push_segment(geo, &s0);
    m3_object_push_segment(geo, &s1);
    m3_object_push_segment(geo, &s2);
    m3_object_push_segment(geo, &s3);
    m3_object_push_segment(geo, &s4);
    m3_object_push_segment(geo, &s5);
    m3_object_push_segment(geo, &s6);
    m3_object_push_segment(geo, &s7);
    m3_object_push_segment(geo, &s8);
    m3_object_push_segment(geo, &sm0);
    m3_object_push_segment(geo, &s9);
    m3_object_push_segment(geo, &sm1);
    m3_object_push_segment(geo, &s10);
    m3_object_push_segment(geo, &sm2);
    m3_object_push_segment(geo, &s11);

    // -- DEFINE GEOMETRY --
    m3_segment_offset(s0, (m3_vec) { .x = 4, .y = 0, .z = 0 });
    m3_segment_offset(s1, (m3_vec) { .x = 0, .y = 4, .z = 0 });
    m3_segment_offset(s2, (m3_vec) { .x = -4, .y = 0, .z = 0 });
    m3_segment_offset(s3, (m3_vec) { .x = 0, .y = -4, .z = 0 });

    m3_segment_offset(s4, (m3_vec) { .x = 0, .y = 0, .z = 4 });
    m3_segment_offset(s5, (m3_vec) { .x = 4, .y = 0, .z = 0 });
    m3_segment_offset(s6, (m3_vec) { .x = 0, .y = 4, .z = 0 });
    m3_segment_offset(s7, (m3_vec) { .x = -4, .y = 0, .z = 0 });
    m3_segment_offset(s8, (m3_vec) { .x = 0, .y = -4, .z = 0 });

    m3_segment_offset(sm0, (m3_vec) { .x = 4, .y = 0, .z = 0 });
    m3_segment_visible(sm0, false);
    m3_segment_offset(s9, (m3_vec) { .x = 0, .y = 0, .z = -4 });

    m3_segment_offset(sm1, (m3_vec) { .x = 0, .y = 4, .z = 0 });
    m3_segment_visible(sm1, false);
    m3_segment_offset(s10, (m3_vec) { .x = 0, .y = 0, .z = 4 });

    m3_segment_offset(sm2, (m3_vec) { .x = -4, .y = 0, .z = 0 });
    m3_segment_visible(sm2, false);
    m3_segment_offset(s11, (m3_vec) { .x = 0, .y = 0, .z = -4 });

    // -- RENDER --

    // Allocate space for camera to render to
    const int width = 64;
    const int height = 32;

    uint8_t target[width * height / 8];

    const int itts = 24;
    // while (1) {
        for (int i = 0; i <= itts; i++) {

            // Clear frame
            memset(target, 0, sizeof(target));

            // Rotate arrow by some amount
            // m3_quat quat = { 0, 127 * sin(i * 3.14159 / itts), 0, 127 * cos(i * 3.14159 / itts) };
            m3_vec dir = { 127 * cos(i * 2 * 3.14159 / itts), 127 * sin(i * 2 * 3.14159 / itts), -127 / 2 };
            m3_vec up = { 0, 0, 1 };

            printf("(%d, %d, %d) x (%d, %d, %d)\n", dir.x, dir.y, dir.z, up.x, up.y, up.z);

            m3_vec_normalize(&dir);
            m3_vec_normalize(&up);

            printf("i: %d, 2*i/itts: %f\n", i, 2.0 * i / itts);

            m3_quat quat = m3_vec_to_quat(
                dir,
                up
            );

            m3_ocamera_pivot(&camera, quat);

            // -- Render --
            m3_ocamera_render(&camera, &scene, target, width, height, M3_ORIENTATION_HL);

            // Print out rasterized image
            printf("\n");
            for (int y = height - 1; y >= 0; y--) {
                printf("| ");
                for (int x = 0; x < width; x++) {
                    uint16_t index = (y*width + x) / 8;
                    uint8_t row = target[index];

                    if (x == width / 2 && y == height / 2) {
                    if (row & (0x01 << (x % 8)))
                        printf("!!");
                    else printf("::");
                        continue;
                    }
                    
                    if (row & (0x01 << (x % 8)))
                        printf("[]");
                    else printf(". ");
                }
                printf(" |\n");
            }
            printf("\n");

            if (i != itts) usleep(4000000 / (itts + 1));
        }
    // }

    return 0;
}
