#include "micro3d.h"

m3_scene_handle_t scene;
m3_ocamera_handle_t camera;

m3_object_handle_t box;
m3_object_handle_t tri;

// Allocate space for camera to render to
const int width = 32;
const int height = 16;

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

  // Create objects to use segments
  box = m3_object_create(scene);
  tri = m3_object_create(scene);
  m3_object_position(box, (m3_vec) { 0, 0, 0 });
  m3_object_position(tri, (m3_vec) { 0, 0, 0 });

  // Fill objects with segments
  m3_object_push_segment(box, vertical);   // |
  m3_object_push_segment(box, horizontal); // |^
  m3_object_push_segment(box, rts);        // |^
  m3_object_push_segment(box, horizontal); // |#
  m3_object_push_segment(box, vertical);   // |#|

  m3_object_push_segment(tri, vertical);   // |#|
  m3_object_push_segment(tri, horizontal);   // |#|
  m3_object_push_segment(tri, rts);   // |#|


  // Create camera
  camera = m3_ocamera_create();
  m3_ocamera_resize(camera, 32, 16);
  m3_ocamera_position(camera, (m3_vec){ 3, 3, 0 });

  // m3_vec dir = { 1, 0, 0 };
  // m3_vec up = { 0, 0, 1 };

  // m3_vec_normalize(&dir);
  // m3_vec_normalize(&up);
  // m3_quat quat = m3_vec_to_quat(dir, up);

  m3_quat quat = { 0, 0, 0, 1 };
  m3_quat_normalize(&quat);
  m3_ocamera_pivot(camera, quat);
}

const int itts = 12000;
bool toggle = false;

void loop() {
  uint32_t start = millis();

  m3_quat quat = { 0, 0, 127 * sin(start * 3.14159 / itts), 127 * cos(start * 3.14159 / itts) };
  // m3_quat quat = { 0, 0, 89, 89 };
  m3_quat_normalize(&quat);
  m3_ocamera_pivot(camera, quat);

  // Alternately hide/show tri vs. box
  toggle = !toggle;
  m3_object_visible(box, toggle);
  m3_object_visible(tri, !toggle);

  // Clear target
  memset(target, 0, sizeof(target));
  
  m3_ocamera_render(camera, scene, target, width, height, M3_ORIENTATION_VL);

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
        uint8_t row = target[(y*width + x) / 8];

        if (x == width / 2 && y == height / 2) {
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

  // Serial.print("(");
  Serial.print(end - start);
  Serial.println("ms)");

  // ~2fps
  delay(500);
}
