// Entry point for platform.io
#include "micro3d.h"
#include <Arduino.h>
#include <Adafruit_QMC5883P.h>
#include <SPI.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
// #define OLED_MOSI  9
// #define OLED_CLK   10
// #define OLED_DC    11
// #define OLED_CS    8
// #define OLED_RESET 13
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
//   OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
Adafruit_QMC5883P mag;

#define OLED_MOSI  11
#define OLED_CLK   13
#define OLED_DC    8
#define OLED_CS    10
#define OLED_RESET 9

// Magnetometer calibration data
// Stored in format min(x, y, z), max(x, y, z)
float mcal[6] = { -0.31, -0.61, -0.59, 0.82, 0.26, 0.35 };

m3_scene_t scene;
m3_ocamera_t camera;

m3_object_handle_t arrow;
m3_object_handle_t fletching;
m3_object_handle_t point;
m3_segment_handle_t tip;

// Determine how the camera looks down on the arrow
m3_quat camera_inclination = { 1, 0, 0, 5 };

uint8_t ssd1306_buf[SCREEN_WIDTH * SCREEN_HEIGHT / 8];
SPISettings spiSettings(8000000UL, MSBFIRST, SPI_MODE0);

// Setup 3d scene
void setup_scene();

void ssd1306_begin();   // Initialize the ssd1306 device
void ssd1306_display(); // Forward the contents of `ssd1306_buf` to the display
void ssd1306_command(const uint8_t *c, uint8_t n); // Send a list of commands to the display

void setup() {
    Serial.begin(115200);

    setup_scene();
    ssd1306_begin();

    // Prepare output display
    // if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    //     Serial.println(F("SSD1306 allocation failed"));
    //     while (1);
    // }

    // Serial.println("Successfully allocated SSD1306");

    // Prepare magnetometer
    while (!mag.begin()) {
        Serial.println("Failed to find QMC5883L chip!");
        delay(1000);
    }
    Serial.println("Found QMC5883L!");

    // Setup magnetometer
    mag.setMode(QMC5883P_MODE_NORMAL);
    mag.setODR(QMC5883P_ODR_50HZ);
    mag.setOSR(QMC5883P_OSR_4);
    mag.setDSR(QMC5883P_DSR_2);
    mag.setRange(QMC5883P_RANGE_2G);
    mag.setSetResetMode(QMC5883P_SETRESET_ON);
}

m3_object_t scene_objects[3];
m3_segment_t scene_segments[22];

uint8_t arrow_segments[16];
uint8_t fletching_segments[8];
uint8_t point_segments[8];

void setup_scene() {
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

    m3_quat_normalize(&camera_inclination);

    // Create required objects
    arrow = m3_object_create(&scene);
    fletching = m3_object_create(&scene);
    point = m3_object_create(&scene);

    // Assign objects to their buffers
    m3_object_alloc_s(arrow, arrow_segments, sizeof(arrow_segments) / sizeof(*arrow_segments));
    m3_object_alloc_s(fletching, fletching_segments, sizeof(fletching_segments) / sizeof(*fletching_segments));
    m3_object_alloc_s(point, point_segments, sizeof(point_segments) / sizeof(*point_segments));

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
    tip = m3_segment_create(&scene);
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
}

void loop() {

    // Get x/y rotation 
    float x, y, z;
    delay(2);
    if (!mag.getGaussField(&x, &y, &z)) return;

    x = 2 * (x - mcal[0]) / (mcal[3] - mcal[0]) - 1;
    y = 2 * (y - mcal[1]) / (mcal[4] - mcal[1]) - 1;

    float mag = sqrt(x*x + y*y);
    float ucx = x / mag;
    float ucy = y / mag;

    // float ucy = sin(millis() / 1000.0);
    // float ucx = cos(millis() / 1000.0);

    m3_vec dir = { -(int8_t) roundf(127 * ucx), 0, (int8_t) roundf(127 * ucy) };
    m3_vec up = { 0, 0, 1 };

    m3_vec_normalize(&dir);
    m3_vec_normalize(&up);

    m3_quat quat = m3_vec_to_quat(dir, up);
    // m3_quat quat = { 0, 127 * ucx, 0, 127 * ucy };

    m3_quat_normalize(&quat);

    // -- Run "backface culling" --
    m3_vec pivot_helper = { 127, 0, 0 };
    m3_vec_rotate(&pivot_helper, quat);

    m3_object_visible(point, abs(pivot_helper.z) <= 80); // Hide point once occluded
    m3_segment_visible(tip, pivot_helper.z > 80); // Hide tip when forward
    m3_object_visible(fletching, pivot_helper.z < 0); // Hide fletching once occluded

    m3_quat live_camera_inclination = { (int8_t) roundf(camera_inclination.x * ucx), 0, 0, camera_inclination.w };
    m3_quat_normalize(&live_camera_inclination);

    m3_quat_rotate(&quat, live_camera_inclination);
    m3_ocamera_pivot(&camera, quat);
    memset(ssd1306_buf, 0, sizeof(ssd1306_buf));
    m3_ocamera_render(&camera, &scene, ssd1306_buf, SCREEN_WIDTH, SCREEN_HEIGHT, M3_ORIENTATION_VL | M3_ORIENTATION_HFLIP);
    ssd1306_display();
}

void ssd1306_begin() {
    SPI.begin();

    // Setup SPI-related pins
    pinMode(OLED_DC, OUTPUT);
    pinMode(OLED_CS, OUTPUT);
    pinMode(OLED_RESET, OUTPUT);
    pinMode(OLED_MOSI, OUTPUT);
    pinMode(OLED_CLK, OUTPUT);

    // Run reset
    digitalWrite(OLED_RESET, HIGH);
    delay(1);
    digitalWrite(OLED_RESET, LOW);
    delay(10);
    digitalWrite(OLED_RESET, HIGH);

    SPI.beginTransaction(spiSettings);
    digitalWrite(OLED_CS, LOW);

    // Initial data required to setup display
    static const uint8_t PROGMEM init[] = {
        0xAE, 0xD5, 0x80, 0xA8,
        SCREEN_HEIGHT - 1,
        0xD3, 0x00, 0x40, 0x8D, 0x14,
        0x20, 0x00, 0xA1, 0xC8,
        0xDA, 0x12, 0x81, 0xCF,
        0xD9, 0xF1,
        0xDB, 0x40, 0xA4, 0xA6, 0x2E, 0xAF
    };
    ssd1306_command(init, sizeof(init));

    digitalWrite(OLED_CS, HIGH);
    SPI.endTransaction();
}

void ssd1306_display() {
    SPI.beginTransaction(spiSettings);
    digitalWrite(OLED_CS, LOW);

    // Data required to initiate display transaction
    static const uint8_t PROGMEM dlist[] = {
        0x22, 0x00, 0xFF, 0x21,
        0x0, SCREEN_WIDTH - 1
    };
    ssd1306_command(dlist, sizeof(dlist));

    uint16_t count = SCREEN_WIDTH * ((SCREEN_HEIGHT + 7) / 8);
    uint8_t* ptr = ssd1306_buf;

    digitalWrite(OLED_DC, HIGH); // Data mode
    while (count--) {
        SPI.transfer(*ptr++);
    }
    
    digitalWrite(OLED_CS, HIGH);
    SPI.endTransaction();
}

void ssd1306_command(const uint8_t *c, uint8_t n) {
    digitalWrite(OLED_DC, LOW); // Command mode

    // Send each command individually
    while (n--) {
        SPI.transfer(pgm_read_byte(c++));
    }
}

