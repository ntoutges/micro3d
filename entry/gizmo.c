// Entry point for platform.io
#include "../src/micro3d.h"
#include <unistd.h>

#define SCREEN_WIDTH 64 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

m3_scene_t scene;
m3_ocamera_t camera;

m3_object_handle_t geo;
m3_segment_t geo_segments[8];

// Determine how the camera looks down on the arrow
m3_quat camera_inclination = { 1, 0, 0, 5 };

uint8_t render_buf[SCREEN_WIDTH * SCREEN_HEIGHT / 8];

// Setup 3d scene
void setup_scene();
void loop();

int main() {
    setup_scene();

    while (1) loop();

    return 0;
}

m3_object_t scene_objects[3];

void setup_scene() {
    // --- Populate scene with a 3d arrow ---

    // Create the main scene
    m3_scene_create_s(
        &scene,
        scene_objects,
        sizeof(scene_objects) / sizeof(*scene_objects)
    );

    // Create camera
    m3_ocamera_create_s(&camera);
    m3_ocamera_resize(&camera, 32, 16);
    m3_ocamera_position(&camera, (m3_vec){ 1, 0, 0 });

    m3_quat_normalize(&camera_inclination);

    // Create required objects
    geo = m3_object_create(&scene);

    // Assign objects to their buffers
    m3_object_alloc_s(geo, geo_segments, sizeof(geo_segments) / sizeof(*geo_segments));

    // Setup object hierarchy

    // Setup object data
    m3_segment_handle_t temp;

    // Define gizmo
    // X-axis
    m3_object_push_segment(geo, &temp);
    m3_segment_offset(temp, (m3_vec){ 7, 0, 0 });
    m3_segment_color(temp, M3_COLOR_FULL);

    // Reset to origin
    m3_object_push_segment(geo, &temp);
    m3_segment_offset(temp, (m3_vec){ 0, 0, 0 });
    m3_segment_color(temp, M3_COLOR_INVISIBLE);
    m3_segment_absolute(temp, true);

    // Y-axis
    m3_object_push_segment(geo, &temp);
    m3_segment_offset(temp, (m3_vec){ 0, 7, 0 });
    m3_segment_color(temp, M3_COLOR_DIM);

    // Reset to origin
    m3_object_push_segment(geo, &temp);
    m3_segment_offset(temp, (m3_vec){ 0, 0, 0 });
    m3_segment_color(temp, M3_COLOR_INVISIBLE);
    m3_segment_absolute(temp, true);

    // Z-axis
    m3_object_push_segment(geo, &temp);
    m3_segment_offset(temp, (m3_vec){ 0, 0, 7 });
    m3_segment_color(temp, M3_COLOR_DARK);
}

#define itts 24 // Steps per rotation

int now = 0;
void loop() {

    // Clear render buffer for next pass
    memset(render_buf, 0, sizeof(render_buf));

    // uint8_t i = millis() * (itts / 2000.0); // 2 seconds per rotation
    uint8_t i = now * (itts / 2000.0); // 2 seconds per rotation

    // Orbit around gizmo, looking down at it from an angle
    m3_vec dir = { 127 * cos(i * 2 * 3.14159 / (itts)), 127 * sin(i * 2 * 3.14159 / (itts)), 127 / 2 };
    m3_vec up = { 0, 0, 1 };

    m3_vec_normalize(&dir);
    m3_vec_normalize(&up);

    m3_quat quat = m3_vec_to_quat(dir, up);

    m3_quat_normalize(&quat);

    m3_ocamera_pivot(&camera, quat);
    m3_ocamera_render(&camera, &scene, render_buf, SCREEN_WIDTH, SCREEN_HEIGHT, M3_ORIENTATION_HL | M3_ORIENTATION_VFLIP);


    // Push new frame to display via Serial
    // Serial.write(render_buf, sizeof(render_buf));
    // Serial.flush();

    char buf[(SCREEN_WIDTH + 1) * SCREEN_HEIGHT + 1];
    int idx = 0;
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH / 8; x++) {
            uint8_t byte = render_buf[y * (SCREEN_WIDTH / 8) + x];
            for (int b = 0; b < 8; b++) {
                buf[idx++] = (byte & (1 << (b))) ? '#' : ' ';
            }
        }
        buf[idx++] = '\n';
    }
    buf[idx++] = '\0';
    printf("---------- %d ----------\n%s", now, buf);

    // Don't overload serial output
    // delay(50);
    sleep(1);
    now += 100;
}
