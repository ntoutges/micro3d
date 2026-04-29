#include "../src/micro3d.h"
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#include "../mongoose/mongoose.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

m3_scene_t scene;
m3_ocamera_t camera;

m3_object_handle_t geo;
m3_segment_t geo_segments[16];

uint8_t render_buf[SCREEN_WIDTH * SCREEN_HEIGHT / 8];

struct mg_mgr mgr;

// Setup 3d scene
void setup_scene();
void loop();

static void handler(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        // Upgrade ALL incoming HTTP requests to WebSocket
        mg_ws_upgrade(c, (struct mg_http_message *) ev_data, NULL);
    } 
    else if (ev == MG_EV_WS_OPEN) {
        printf("Client connected\n");
    }
}

struct timespec start, last, now;
int main() {
    setup_scene();

    mg_mgr_init(&mgr);

    // Listen for WebSocket connections
    mg_http_listen(&mgr, "ws://0.0.0.0:3000", handler, NULL);
    printf("WebSocket server started on ws://0.0.0.0:3000\n");

    clock_gettime(CLOCK_MONOTONIC, &start);
    last = start;
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

    // Create required objects
    geo = m3_object_create(&scene);

    // Assign objects to their buffers
    m3_object_alloc_s(geo, geo_segments, sizeof(geo_segments) / sizeof(*geo_segments));

    // Setup object hierarchy

    // Define cube
    m3_segment_handle_t so, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, sm0, sm1, sm2;

    m3_object_push_segment(geo, &so);
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
    m3_segment_color(s0, M3_COLOR_DIM);
    m3_segment_offset(s1, (m3_vec) { .x = 0, .y = 4, .z = 0 });
    m3_segment_color(s1, M3_COLOR_DIM);
    m3_segment_offset(s2, (m3_vec) { .x = -4, .y = 0, .z = 0 });
    m3_segment_color(s2, M3_COLOR_DIM);
    m3_segment_offset(s3, (m3_vec) { .x = 0, .y = -4, .z = 0 });
    m3_segment_color(s3, M3_COLOR_DIM);

    m3_segment_offset(s4, (m3_vec) { .x = 0, .y = 0, .z = 4 });
    m3_segment_offset(s5, (m3_vec) { .x = 4, .y = 0, .z = 0 });
    m3_segment_offset(s6, (m3_vec) { .x = 0, .y = 4, .z = 0 });
    m3_segment_offset(s7, (m3_vec) { .x = -4, .y = 0, .z = 0 });
    m3_segment_offset(s8, (m3_vec) { .x = 0, .y = -4, .z = 0 });

    m3_segment_offset(sm0, (m3_vec) { .x = 4, .y = 0, .z = 0 });
    m3_segment_color(sm0, M3_COLOR_INVISIBLE);
    m3_segment_offset(s9, (m3_vec) { .x = 0, .y = 0, .z = -4 });

    m3_segment_offset(sm1, (m3_vec) { .x = 0, .y = 4, .z = 0 });
    m3_segment_color(sm1, M3_COLOR_INVISIBLE);
    m3_segment_offset(s10, (m3_vec) { .x = 0, .y = 0, .z = 4 });

    m3_segment_offset(sm2, (m3_vec) { .x = -4, .y = 0, .z = 0 });
    m3_segment_color(sm2, M3_COLOR_INVISIBLE);
    m3_segment_offset(s11, (m3_vec) { .x = 0, .y = 0, .z = -4 });
}

#define itts 24 // Steps per rotation

void loop() {
    clock_gettime(CLOCK_MONOTONIC, &now);
    mg_mgr_poll(&mgr, 10);

    uint64_t now_ms = (now.tv_sec - start.tv_sec) * 1000 + now.tv_nsec / 1000000;
    uint64_t last_ms = (last.tv_sec - start.tv_sec) * 1000 + last.tv_nsec / 1000000;

    if (now_ms < last_ms + 100) return;
    last = now;

    // Clear render buffer for next pass
    memset(render_buf, 0, sizeof(render_buf));

    // uint8_t i = millis() * (itts / 2000.0); // 2 seconds per rotation
    float i = now_ms / 1000.0 * (itts / 3.0); // 2 seconds per rotation

    // Orbit around gizmo, looking down at it from an angle
    m3_vec dir = { 127 * cos(i * 2 * 3.14159 / (itts)), 127 * sin(i * 2 * 3.14159 / (itts)), 127 / 2 };
    m3_vec up = { 0, 0, 1 };

    m3_vec_normalize(&dir);
    m3_vec_normalize(&up);

    m3_quat quat = m3_vec_to_quat(dir, up);

    m3_quat_normalize(&quat);

    m3_ocamera_pivot(&camera, quat);
    m3_ocamera_render(&camera, &scene, render_buf, SCREEN_WIDTH, SCREEN_HEIGHT, M3_ORIENTATION_HL);

    // Push new frame to display via socket
    for (struct mg_connection *c = mgr.conns; c; c = c->next) {
        if (c->is_websocket) {
            mg_ws_send(c, render_buf, sizeof(render_buf), WEBSOCKET_OP_BINARY);
        }
    }
}
