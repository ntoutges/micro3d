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

Adafruit_QMC5883P mag;

void setup() {
  Serial.begin(115200);

  // Create a test scene
  scene = m3_scene_create_d(8, 4);

  // Populate scene with a 1x1 box
  // Create base segments
  m3_segment_handle_t body = m3_segment_create(scene);
  m3_segment_handle_t arrow_l  = m3_segment_create(scene);
  m3_segment_handle_t arrow_r = m3_segment_create(scene);
  m3_segment_handle_t arrow_b = m3_segment_create(scene);

  // Populate base segments
  m3_segment_offset(body, (m3_vec) { 7, 0, 0 });
  m3_segment_offset(arrow_l, (m3_vec) { -3, 2, 0 });
  m3_segment_offset(arrow_r, (m3_vec) { 3, 2, 0 });
  m3_segment_offset(arrow_b, (m3_vec) { 0, -4, 0 });
  m3_segment_visible(arrow_b, false);

  // Create objects to use segments
  arrow = m3_object_create(scene);
  m3_object_position(arrow, (m3_vec) { 0, 0, 0 });

  // Fill arrow with segments
  m3_object_push_segment(arrow, body);
  m3_object_push_segment(arrow, body);
  m3_object_push_segment(arrow, arrow_l);
  m3_object_push_segment(arrow, arrow_b);
  m3_object_push_segment(arrow, arrow_r);

  // Create camera
  camera = m3_ocamera_create();
  m3_ocamera_resize(camera, 32, 16);
  m3_ocamera_position(camera, (m3_vec){ 7, 0, 0 });

  m3_quat quat = { 0, 0, 0, 1 };
  m3_quat_normalize(&quat);
  m3_ocamera_pivot(camera, quat);

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

float min_x = 0.21;
float min_y = -0.78;
float max_x = 0.79;
float max_y = -0.21;

void loop() {

  // Get x/y rotation 
  float x, y, z;
  delay(2);
  if (!mag.getGaussField(&x, &y, &z)) return;
  float cx = (x - min_x) / (max_x - min_x) * 2 - 1;
  float cy = (y - min_y) / (max_y - min_y) * 2 - 1;

  float mag = sqrt(cx*cx + cy*cy);
  float ucx = cx / mag;
  float ucy = cy / mag;

  uint32_t start = millis();

  // Create vector from cx/cy
  // m3_vec rotation = { ucx * 127, 0, ucy * 127 };
  // m3_vec up = { 0, 0, 127 };

  // m3_quat quat = m3_vec_to_quat(rotation, up);
  m3_quat quat = { 0, ucx * 127, 0, ucy * 127 };
  m3_quat_normalize(&quat);

  m3_ocamera_pivot(camera, quat);

  display.clearDisplay();  
  m3_ocamera_render(camera, scene, display.getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT, M3_ORIENTATION_VL | M3_ORIENTATION_HFLIP);
  display.display();

  delay(100);
}
