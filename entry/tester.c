// Used to test Micro3d library

#include "../src/micro3d.h"
#include "stdio.h"

#include <unistd.h>

m3_scene_t scene;
m3_ocamera_t camera;

m3_object_handle_t arrow;
m3_object_handle_t fletching;
m3_object_handle_t point;
m3_segment_handle_t tip;

m3_object_t scene_objects[3];
m3_segment_t scene_segments[22];

uint8_t arrow_segments[16];
uint8_t fletching_segments[8];
uint8_t point_segments[8];

int main() {
    // --- Populate scene with a 3d arrow ---

    // Create the main scene
    m3_scene_create_s(
        &scene,
        scene_segments,
        sizeof(scene_segments) / sizeof(*scene_segments),
        scene_objects,
        sizeof(scene_objects) / sizeof(*scene_objects)
    );

    // Create camera
    m3_ocamera_create_s(&camera);
    m3_ocamera_resize(&camera, 32, 16);
    m3_ocamera_position(&camera, (m3_vec){ 1, 0, 0 });

    // Create required objects
    arrow = m3_object_create(&scene);
    fletching = m3_object_create(&scene);
    point = m3_object_create(&scene);

    // Assign objects to their buffers
    m3_object_alloc_s(arrow, arrow_segments, sizeof(arrow_segments));
    m3_object_alloc_s(fletching, fletching_segments, sizeof(fletching_segments));
    m3_object_alloc_s(point, point_segments, sizeof(point_segments));

    // Setup object hierarchy
    m3_object_pset(fletching, arrow);
    m3_object_pset(point, arrow);

    // Setup object data
    m3_object_position(fletching, (m3_vec){ -7, 0, 0 });
    m3_object_position(point, (m3_vec){ 7, 0, 0 });
    m3_object_rlock(point, M3_RLOCK_X);

    // -- Create segments --
    // Fletching
    m3_segment_handle_t af0 = m3_segment_create(&scene);
    m3_segment_handle_t af1 = m3_segment_create(&scene);
    m3_segment_handle_t af2 = m3_segment_create(&scene);
    m3_segment_handle_t af3 = m3_segment_create(&scene);

    // Body
    m3_segment_handle_t ab0 = m3_segment_create(&scene);
    m3_segment_handle_t ab1 = m3_segment_create(&scene);
    m3_segment_handle_t tip = m3_segment_create(&scene);
    m3_segment_handle_t abr = m3_segment_create(&scene);
    m3_segment_handle_t abp = m3_segment_create(&scene);

    // Ring
    m3_segment_handle_t ar0 = m3_segment_create(&scene);
    m3_segment_handle_t ar1 = m3_segment_create(&scene);
    m3_segment_handle_t ar2 = m3_segment_create(&scene);
    m3_segment_handle_t ar3 = m3_segment_create(&scene);
    m3_segment_handle_t ar4 = m3_segment_create(&scene);
    m3_segment_handle_t ar5 = m3_segment_create(&scene);
    m3_segment_handle_t ar6 = m3_segment_create(&scene);
    m3_segment_handle_t ar7 = m3_segment_create(&scene);
    m3_segment_handle_t arp = m3_segment_create(&scene);

    // Point
    m3_segment_handle_t ap0 = m3_segment_create(&scene);
    m3_segment_handle_t ap1 = m3_segment_create(&scene);
    m3_segment_handle_t app = m3_segment_create(&scene);

    // Origin
    m3_segment_handle_t aorigin = m3_segment_create(&scene);

    // -- Populate segments --
    // Fletching
    m3_segment_offset(af0, (m3_vec){ -2, 2, 2 });
    m3_segment_offset(af1, (m3_vec){ -2, 2, -2 });
    m3_segment_offset(af2, (m3_vec){ -2, -2, 2 });
    m3_segment_offset(af3, (m3_vec){ -2, -2, -2 });

    // Body
    m3_segment_offset(ab0, (m3_vec){ 7, 0, 0 });
    m3_segment_offset(ab1, (m3_vec){ 6, 0, 0 });
    m3_segment_visible(ab1, false);
    m3_segment_offset(tip, (m3_vec){ 0, 0, 0 });
    m3_segment_offset(abr, (m3_vec){ -6, 0, 0 });
    m3_segment_visible(abr, false);
    m3_segment_offset(abp, (m3_vec){ -7, 0, 0 });
    m3_segment_visible(abp, false);
    
    // Ring
    m3_segment_offset(ar0, (m3_vec){ 0, 2, 2 });
    m3_segment_offset(ar1, (m3_vec){ 0, 0, 4 });
    m3_segment_offset(ar2, (m3_vec){ 0, -2, 2 });
    m3_segment_offset(ar3, (m3_vec){ 0, -4, 0 });
    m3_segment_offset(ar4, (m3_vec){ 0, -2, -2 });
    m3_segment_offset(ar5, (m3_vec){ 0, 0, -4 });
    m3_segment_offset(ar6, (m3_vec){ 0, 2, -2 });
    m3_segment_offset(ar7, (m3_vec){ 0, 4, 0 });
    m3_segment_offset(arp, (m3_vec){ 0, 2, -4 });
    m3_segment_visible(arp, false);

    // Point
    m3_segment_offset(ap0, (m3_vec){ -6, 4, 0 });
    m3_segment_offset(ap1, (m3_vec){ -6, -4, 0 });
    m3_segment_offset(app, (m3_vec){ 6, 0, 0 });
    m3_segment_absolute(app, true);
    m3_segment_visible(app, false);

    // Origin
    m3_segment_offset(aorigin, (m3_vec){ 0, 0, 0 });
    m3_segment_absolute(aorigin, true);
    m3_segment_visible(aorigin, false);

    // -- Fill objects with segments --
    // Setup base arrow
    m3_object_push_segment(arrow, abp);
    m3_object_push_segment(arrow, ab0);
    m3_object_push_segment(arrow, ab0);
    m3_object_push_segment(arrow, ab1);
    m3_object_push_segment(arrow, tip);
    m3_object_push_segment(arrow, abr);
    m3_object_push_segment(arrow, arp);
    m3_object_push_segment(arrow, ar0);
    m3_object_push_segment(arrow, ar1);
    m3_object_push_segment(arrow, ar2);
    m3_object_push_segment(arrow, ar3);
    m3_object_push_segment(arrow, ar4);
    m3_object_push_segment(arrow, ar5);
    m3_object_push_segment(arrow, ar6);
    m3_object_push_segment(arrow, ar7);

    // Setup hideable fletching
    m3_object_push_segment(fletching, af0);
    m3_object_push_segment(fletching, aorigin);
    m3_object_push_segment(fletching, af1);
    m3_object_push_segment(fletching, aorigin);
    m3_object_push_segment(fletching, af2);
    m3_object_push_segment(fletching, aorigin);
    m3_object_push_segment(fletching, af3);

    // Setup hideable point
    m3_object_push_segment(point, app);
    m3_object_push_segment(point, ap0);
    m3_object_push_segment(point, app);
    m3_object_push_segment(point, ap1);

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
            m3_vec dir = { 127 * cos(i * 2 * 3.14159 / itts), 0, 127 * sin(i * 2 * 3.14159 / itts) };
            m3_vec up = { 127 * sin(i * 2 * 3.14159 / itts), 0, -127 * cos(i * 2 * 3.14159 / itts)  };

            m3_vec_normalize(&dir);
            m3_vec_normalize(&up);

            printf("i: %d, 2*i/itts: %f\n", i, 2.0 * i / itts);

            m3_quat quat = m3_vec_to_quat(
                dir,
                up
            );

            // m3_quat quat = { 0, 0, 0, 1 };
            m3_quat_normalize(&quat);
            m3_object_pivot(arrow, quat);

            // printf("(%d, %d, %d, %d)\n", quat.x, quat.y, quat.z, quat.w);

            // -- Run "backface culling" --
            m3_vec pivot_helper = { 127, 0, 0 };
            m3_vec_rotate(&pivot_helper, quat);

            m3_object_visible(point, abs(pivot_helper.z) < 90); // Hide point once occluded
            m3_segment_visible(tip, pivot_helper.z <= -90); // Hide tip when forward
            m3_object_visible(fletching, pivot_helper.z >= 0); // Hide point once occluded
            

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
