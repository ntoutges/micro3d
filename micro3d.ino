#include "micro3d.h"

m3_scene_handle_t scene;
m3_object_handle_t box;
m3_ocamera_handle_t camera;

// Allocate space for camera to render to
const int width = 128;
const int height = 64;

uint8_t target[width * height / 8];

void setup() {
  Serial.begin(115200);

  // Create a test scene
  scene = m3_scene_create_d(8, 4);

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
  box = m3_object_create(scene);
  m3_object_position(box, (m3_vec) { 0, 0, 0 });

  // Fill object with segments
  m3_object_push_segment(box, vertical);   // |
  m3_object_push_segment(box, horizontal); // |^
  m3_object_push_segment(box, rts);        // |^
  m3_object_push_segment(box, horizontal); // |#
  m3_object_push_segment(box, vertical);   // |#|

  // Create camera
  camera = m3_ocamera_create();
  m3_ocamera_resize(camera, 16, 16);
  m3_ocamera_position(camera, (m3_vec){ 0, 0, 0 });

  // m3_vec dir = { 1, 0, 0 };
  // m3_vec up = { 0, 0, 1 };

  // m3_vec_normalize(&dir);
  // m3_vec_normalize(&up);
  // m3_quat quat = m3_vec_to_quat(dir, up);

  m3_quat quat = { 0, 0, 0, 1 };
  m3_quat_normalize(&quat);
  m3_ocamera_pivot(camera, quat);
}

int t = 0;

void loop() {
  uint32_t start = millis();

  m3_quat quat = { 0, 0, sin(t * 3.1415926 / 36) * 127, cos(t * 3.1415926 / 36) * 127 };
  m3_quat_normalize(&quat);
  m3_ocamera_pivot(camera, quat);
  t++;

  // Clear target
  memset(target, 0, sizeof(target));
  
  m3_ocamera_render(camera, scene, target, width, height);

  uint32_t end = millis();

  // Print out rasterized image
  Serial.print("+-");
  for (int x = 0; x < width; x++) {
    Serial.print("--");
  }
  Serial.println("-+");

  for (int y = height - 1; y >= 0; y--) {
    Serial.print("| ");
    for (int x = 0; x < width; x++) {
        uint8_t row = target[(y*height + x) / 8];

        if (x == height / 2 && y == height / 2) {
            Serial.print("::");
            continue;
        }
        
        if (row & (0x01 << x % 8))
            Serial.print("||");
        else Serial.print("  ");
    }
    Serial.println(" |");
  }
  // Print out rasterized image
  Serial.print("+-");
  for (int x = 0; x < width; x++) {
    Serial.print("--");
  }
  Serial.println("-+");

  Serial.print("(");
  Serial.print(end - start);
  Serial.println("ms)");

  // ~4fps
  delay(500);
}
