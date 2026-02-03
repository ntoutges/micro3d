#include "micro3d.h"
#include <Adafruit_QMC5883P.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

m3_scene_handle_t scene;
m3_ocamera_handle_t camera;

m3_object_handle_t arrow;
m3_object_handle_t fletching;
m3_object_handle_t point;

Adafruit_QMC5883P mag;

void setup() {
  Serial.begin(115200);

  setup_scene();

  // Prepare output display
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (1);
  }

  display.clearDisplay();
  display.display();
  
  // Monitor the real world
  while (!mag.begin()) {
    Serial.println("Failed to find QMC5883L chip!");
    delay(1000);
  }
  Serial.println("Found QMC5883L!");

  mag.setMode(QMC5883P_MODE_NORMAL);
  mag.setODR(QMC5883P_ODR_50HZ);
  mag.setOSR(QMC5883P_OSR_4);
  mag.setDSR(QMC5883P_DSR_2);
  mag.setRange(QMC5883P_RANGE_2G);
  mag.setSetResetMode(QMC5883P_SETRESET_ON);
}

m3_object_t scene_objects[3];
m3_segment_t scene_segments[18];

void setup_scene() {
  // --- Populate scene with a 3d arrow ---

  // Create the main scene
  m3_scene_create_s(
    scene,
    scene_segments,
    sizeof(scene_segments) / sizeof(*scene_segments),
    scene_objects,
    sizeof(scene_objects) / sizeof(*scene_objects)
  );

  // Create camera
  m3_ocamera_create_s(camera);
  m3_ocamera_resize(camera, 16, 8);
  m3_ocamera_position(camera, (m3_vec){ 4, 0, 0 });

  // Create required objects
  arrow = m3_object_create(scene);
  fletching = m3_object_create(scene);
  point = m3_object_create(scene);

  // Setup object hierarchy
  m3_object_pset(fletching, arrow);
  m3_object_pset(point, arrow);

  // Setup object offsets
  m3_object_position(fletching, (m3_vec){ 0, 0, 0 });
  m3_object_position(point, (m3_vec){ 7, 0, 0 });

  // -- Create segments --
  // Fletching
  m3_segment_handle_t af0 = m3_segment_create(scene);
  m3_segment_handle_t af1 = m3_segment_create(scene);
  m3_segment_handle_t af2 = m3_segment_create(scene);
  m3_segment_handle_t af3 = m3_segment_create(scene);

  // Body
  m3_segment_handle_t abody = m3_segment_create(scene);

  // Ring
  m3_segment_handle_t ar0 = m3_segment_create(scene);
  m3_segment_handle_t ar1 = m3_segment_create(scene);
  m3_segment_handle_t ar2 = m3_segment_create(scene);
  m3_segment_handle_t ar3 = m3_segment_create(scene);
  m3_segment_handle_t ar4 = m3_segment_create(scene);
  m3_segment_handle_t ar5 = m3_segment_create(scene);
  m3_segment_handle_t ar6 = m3_segment_create(scene);
  m3_segment_handle_t ar7 = m3_segment_create(scene);
  m3_segment_handle_t arp = m3_segment_create(scene);

  // Point
  m3_segment_handle_t ap0 = m3_segment_create(scene);
  m3_segment_handle_t ap1 = m3_segment_create(scene);
  m3_segment_handle_t app = m3_segment_create(scene);

  // Origin
  m3_segment_handle_t aorigin = m3_segment_create(scene);

  // -- Populate segments --
  // Fletching
  m3_segment_offset(af0, (m3_vec){ -1, 1, 1 });
  m3_segment_offset(af1, (m3_vec){ -1, 1, -1 });
  m3_segment_offset(af2, (m3_vec){ -1, -1, 1 });
  m3_segment_offset(af3, (m3_vec){ -1, -1, -1 });

  // Body
  m3_segment_offset(abody, (m3_vec){ 7, 0, 0 });
  
  // Ring
  m3_segment_offset(ar0, (m3_vec){ 0, 1, 1 });
  m3_segment_offset(ar1, (m3_vec){ 0, 0, 2 });
  m3_segment_offset(ar2, (m3_vec){ 0, -1, 1 });
  m3_segment_offset(ar3, (m3_vec){ 0, -2, 0 });
  m3_segment_offset(ar4, (m3_vec){ 0, -1, -1 });
  m3_segment_offset(ar5, (m3_vec){ 0, 0, -2 });
  m3_segment_offset(ar6, (m3_vec){ 0, 1, -1 });
  m3_segment_offset(ar7, (m3_vec){ 0, 2, 0 });
  m3_segment_offset(arp, (m3_vec){ 0, 1, -2 });
  m3_segment_visible(arp, false);

  // Point
  m3_segment_offset(ap0, (m3_vec){ -3, 2, 0 });
  m3_segment_offset(ap1, (m3_vec){ -3, -2, 0 });
  m3_segment_offset(app, (m3_vec){ 3, 0, 0 });
  m3_segment_absolute(app, true);
  m3_segment_visible(app, false);

  // Origin
  m3_segment_offset(aorigin, (m3_vec){ 0, 0, 0 });
  m3_segment_absolute(aorigin, true);
  m3_segment_visible(aorigin, false);

  // -- Fill objects with segments --
  // Setup base arrow
  m3_object_push_segment(arrow, abody);
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
}

float min_x = 0.21;
float min_y = -0.78;
float max_x = 0.79;
float max_y = -0.21;

void loop() {

  // Get x/y rotation 
  // float x, y, z;
  // delay(2);
  // if (!mag.getGaussField(&x, &y, &z)) return;
  // float cx = (x - min_x) / (max_x - min_x) * 2 - 1;
  // float cy = (y - min_y) / (max_y - min_y) * 2 - 1;

  // float mag = sqrt(cx*cx + cy*cy);
  // float ucx = cx / mag;
  // float ucy = cy / mag;

  float ucy = sin(millis() / 1000.0);
  float ucx = cos(millis() / 1000.0);

  // m3_quat quat = m3_vec_to_quat(rotation, up);
  m3_quat quat = { 0, ucx * 127, 0, ucy * 127 };
  m3_quat_normalize(&quat);

  // -- Run "backface culling" --
  m3_vec pivot_helper = { 127, 0, 0 };
  m3_vec_rotate(&pivot_helper, quat);

  m3_object_visible(point, abs(pivot_helper.z) < 100); // Hide point once occluded
  m3_object_visible(fletching, pivot_helper.z <= 0); // Hide point once occluded

  m3_ocamera_pivot(camera, quat);

  display.clearDisplay();  
  m3_ocamera_render(camera, scene, display.getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT, M3_ORIENTATION_VL | M3_ORIENTATION_HFLIP);
  display.display();

  delay(100);
}
