// Entry point for platform.io
#include "micro3d.h"
#include "../src/cmd/cmd.h"
#include <Arduino.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define MAX_OBJ_CT 4
#define MAX_SEG_CT 16

m3_scene_t scene;
m3_ocamera_t camera;

// Object/segment buffers
m3_object_t obj_buf[MAX_OBJ_CT];
m3_segment_t seg_buf[MAX_OBJ_CT][MAX_SEG_CT];

uint8_t render_buf[SCREEN_WIDTH * SCREEN_HEIGHT / 8];

cmd_t myCmd;
cmd_entry_t cmd_ebuf[4]; // Entry buf
uint8_t cmd_bbuf[80];    // Recv buffer

// Setup 3d scene
void setup_scene();

void handle_response(uint8_t rid, bool error, uint8_t response);
void handle_cam(void*);
void handle_obj(void*);
void handle_seg(void*);

void setup() {
    Serial.begin(115200);
    setup_scene();
}

m3_object_t scene_objects[3];

void setup_scene() {
    // Initialize command handler
    myCmd = cmd(cmd_ebuf, sizeof(cmd_ebuf) / sizeof(*cmd_ebuf), cmd_bbuf, sizeof(cmd_bbuf) / sizeof(*cmd_bbuf), '!');
    cmd_attach(&myCmd, "cam", handle_cam, NULL);
    cmd_attach(&myCmd, "obj", handle_obj, NULL);
    cmd_attach(&myCmd, "seg", handle_seg, NULL);

    // Create the main scene
    m3_scene_create_s(
        &scene,
        obj_buf,
        sizeof(obj_buf) / sizeof(*obj_buf)
    );

    // Create camera
    m3_ocamera_create_s(&camera, 4);
    m3_ocamera_resize(&camera, 128, 64);
}

void loop() {

    // Check for commands
    while (Serial.available()) {
        cmd_recv(&myCmd, Serial.read());
    }

    // Clear render buffer for next pass
    memset(render_buf, 0, sizeof(render_buf));

    m3_ocamera_render(&camera, &scene, render_buf, SCREEN_WIDTH, SCREEN_HEIGHT, M3_ORIENTATION_HL);

    // Push new frame to display via Serial
    Serial.write(0x00); // Command byte for display data
    Serial.write(render_buf, sizeof(render_buf));
    Serial.flush();
}

void handle_response(uint8_t rid, bool error, uint8_t response) {
    uint8_t buf[2];

    buf[0] = 0x80 | (error ? 0x40 : 0x00) | (rid & 0x3F);
    buf[1] = response;

    Serial.write(buf, sizeof(buf));
}

void handle_cam(void*) {
    uint8_t rid = cmd_ogeti(&myCmd, 1, 0xFF);
    const char* subcmd = cmd_ogets(&myCmd, 2, "");

    if (rid >= 64) return; // Invalid rid

    if (strcmp(subcmd, "pos") == 0) {
        int8_t x = cmd_ogeti(&myCmd, 3, 0);
        int8_t y = cmd_ogeti(&myCmd, 4, 0);
        int8_t z = cmd_ogeti(&myCmd, 5, 0);

        m3_ocamera_position(&camera, (m3_vec){ x, y, z });
        handle_response(rid, false, 0);
        return;
    }

    if (strcmp(subcmd, "pivot") == 0) {
        int8_t x = cmd_ogeti(&myCmd, 3, 0);
        int8_t y = cmd_ogeti(&myCmd, 4, 0);
        int8_t z = cmd_ogeti(&myCmd, 5, 0);
        int8_t w = cmd_ogeti(&myCmd, 6, 0);

        m3_ocamera_pivot(&camera, (m3_quat){ x, y, z, w });
        handle_response(rid, false, 0);
        return;
    }

    if (strcmp(subcmd, "resize") == 0) {
        uint8_t width = cmd_ogeti(&myCmd, 3, 0);
        uint8_t height = cmd_ogeti(&myCmd, 4, 0);

        m3_ocamera_resize(&camera, width, height);
        handle_response(rid, false, 0);
        return;
    }

    if (strcmp(subcmd, "clear") == 0) {
        m3_scene_clear(&scene);

        handle_response(rid, false, 0);
        return;
    }


    handle_response(rid, true, 19); // Unhandled command
}

void handle_obj(void*) {
    uint8_t rid = cmd_ogeti(&myCmd, 1, 0xFF);
    const char* subcmd = cmd_ogets(&myCmd, 2, "");

    if (strcmp(subcmd, "create") == 0) {
        m3_object_handle_t obj = m3_object_create(&scene);
        
        // Failed to allocate object
        if (!m3_object_exists(obj)) {
            handle_response(rid, true, 20);
            return;
        }

        // Allocate segment memory for object based on internal id
        m3_object_alloc_s(obj, seg_buf[obj.id], sizeof(*seg_buf) / sizeof(**seg_buf));

        handle_response(rid, false, obj.id);
        return;
    }

    if (strcmp(subcmd, "parent") == 0) {
        uint8_t id = cmd_ogeti(&myCmd, 3, 0);
        uint8_t parent = cmd_ogeti(&myCmd, 4, 0);

        // Ensure object exists
        m3_object_handle_t obj = m3_scene_object_get(&scene, id);
        if (!m3_object_exists(obj)) {
            handle_response(rid, true, 21);
            return;
        }

        // Ensure parent exists
        m3_object_handle_t objp = m3_scene_object_get(&scene, parent);
        if (!m3_object_exists(objp)) {
            handle_response(rid, true, 22);
            return;
        }

        // Success!
        m3_object_pset(obj, objp);
        handle_response(rid, false, 0);
        return;
    }

    if (strcmp(subcmd, "pos") == 0) {
        uint8_t id = cmd_ogeti(&myCmd, 3, 0);
        int8_t x = cmd_ogeti(&myCmd, 4, 0);
        int8_t y = cmd_ogeti(&myCmd, 5, 0);
        int8_t z = cmd_ogeti(&myCmd, 6, 0);

        // Ensure object exists
        m3_object_handle_t obj = m3_scene_object_get(&scene, id);
        if (!m3_object_exists(obj)) {
            handle_response(rid, true, 23);
            return;
        }

        // Sucecss!
        m3_object_position(obj, (m3_vec){ .x = x, .y = y, .z = z });
        handle_response(rid, false, 0);
        return;
    }
    
    if (strcmp(subcmd, "pivot") == 0) {
        uint8_t id = cmd_ogeti(&myCmd, 3, 0);
        int8_t x = cmd_ogeti(&myCmd, 4, 0);
        int8_t y = cmd_ogeti(&myCmd, 5, 0);
        int8_t z = cmd_ogeti(&myCmd, 6, 0);
        int8_t w = cmd_ogeti(&myCmd, 7, 0);

        // Ensure object exists
        m3_object_handle_t obj = m3_scene_object_get(&scene, id);
        if (!m3_object_exists(obj)) {
            handle_response(rid, true, 24);
            return;
        }
        
        // Success!
        m3_object_pivot(obj, (m3_quat){ .x = x, .y = y, .z = z, .w = w });
        handle_response(rid, false, 0);
        return;
    }

    if (strcmp(subcmd, "visible") == 0) {
        uint8_t id = cmd_ogeti(&myCmd, 3, 0);
        uint8_t visible = cmd_ogetb(&myCmd, 4, true);

        // Ensure object exists
        m3_object_handle_t obj = m3_scene_object_get(&scene, id);
        if (!m3_object_exists(obj)) {
            handle_response(rid, true, 25);
            return;
        }

        // Success!
        m3_object_visible(obj, visible);
        handle_response(rid, false, 0);
        return;
    }

    if (strcmp(subcmd, "rlock") == 0) {
        uint8_t id = cmd_ogeti(&myCmd, 3, 0);
        bool x = cmd_ugetb(&myCmd, "x", false);
        bool y = cmd_ugetb(&myCmd, "y", false);

        // Ensure object exists
        m3_object_handle_t obj = m3_scene_object_get(&scene, id);
        if (!m3_object_exists(obj)) {
            handle_response(rid, true, 26);
            return;
        }

        // Success!
        m3_object_rlock(obj, (x ? M3_RLOCK_X : 0x00) | (y ? M3_RLOCK_Y : 0x00));
        handle_response(rid, false, 0);
        return;
    }

    if (strcmp(subcmd, "clear") == 0) {
        uint8_t id = cmd_ogeti(&myCmd, 3, 0);

        // Ensure object exists
        m3_object_handle_t obj = m3_scene_object_get(&scene, id);
        if (!m3_object_exists(obj)) {
            handle_response(rid, true, 27);
            return;
        }

        // Success!
        m3_object_clear(obj);
        handle_response(rid, false, 0);
        return;
    }

    handle_response(rid, true, 39); // Unhandled command
}

void handle_seg(void*) {
    uint8_t rid = cmd_ogeti(&myCmd, 1, 0xFF);
    const char* subcmd = cmd_ogets(&myCmd, 2, "");
    uint8_t objId = cmd_ogeti(&myCmd, 3, 0);

    // Invalid obejct id
    m3_object_handle_t obj = m3_scene_object_get(&scene, objId);
    if (!m3_object_exists(obj)) {
        handle_response(rid, true, 40);
        return;
    }

    if (strcmp(subcmd, "create") == 0) {

        // Attempt to allocate segment
        m3_segment_handle_t seg;
        m3_object_ires_t res = m3_object_push_segment(obj, &seg);
        if (res.err != 0 || !m3_segment_exists(seg)) {
            handle_response(rid, true, 41);
            return;
        }

        // Success!
        handle_response(rid, false, seg.id);
        return;

    }

    // Invalid segment id
    uint8_t id = cmd_ogeti(&myCmd, 4, 0);
    m3_segment_handle_t seg = m3_object_segment_get(obj, id);
    if (!m3_segment_exists(seg)) {
        handle_response(rid, true, 42);
        return;
    }

    if (strcmp(subcmd, "offset") == 0) {
        int8_t x = cmd_ogeti(&myCmd, 5, 0);
        int8_t y = cmd_ogeti(&myCmd, 6, 0);
        int8_t z = cmd_ogeti(&myCmd, 7, 0);

        m3_segment_offset(seg, (m3_vec){ .x = x, .y = y, .z = z });
        handle_response(rid, false, 0);
        return;
    }

    if (strcmp(subcmd, "absolute") == 0) {
        uint8_t absolute = cmd_ogetb(&myCmd, 5, false);

        m3_segment_absolute(seg, absolute);
        handle_response(rid, false, 0);
        return;
    }

    if (strcmp(subcmd, "color") == 0) {
        uint8_t color = cmd_ogeti(&myCmd, 5, M3_COLOR_FULL);

        m3_segment_color(seg, color);
        handle_response(rid, false, 0);
        return;
    }

    handle_response(rid, true, 59); // Unhandled command
}
