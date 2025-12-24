/*
 * Project: AVSN - Slave Camera Unit
 * Logic: Wait for HIGH signal on Pin 12 -> Take Photo -> Save to SD
 */
#include "esp_camera.h"
#include "FS.h"                // SD Card file system
#include "SD_MMC.h"            // SD Card driver

// Pin from Master ESP32
#define PIN_TRIGGER_INPUT 12 

// Camera Pin Definitions (AI Thinker Model)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void setup() {
  Serial.begin(115200);
  pinMode(PIN_TRIGGER_INPUT, INPUT); // Listen for Master

  // Camera Configuration (Standard Boilerplate)
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_UXGA; // High Res
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera Init Failed 0x%x", err);
    return;
  }
  
  // Init SD Card
  if(!SD_MMC.begin()){
    Serial.println("SD Card Mount Failed");
    return;
  }
}

void loop() {
  // Wait for signal from Master ESP32
  if (digitalRead(PIN_TRIGGER_INPUT) == HIGH) {
    Serial.println("Trigger Received! Taking photo...");
    takePhotoAndSave();
    delay(2000); // Prevent double-triggering
  }
}

void takePhotoAndSave() {
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get(); // Snap picture
  if(!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // Create a random file name or timestamped name
  String path = "/capture_" + String(millis()) + ".jpg";
  
  fs::FS &fs = SD_MMC; 
  File file = fs.open(path.c_str(), FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file in writing mode");
  } else {
    file.write(fb->buf, fb->len); // Write image data
    Serial.printf("Saved: %s\n", path.c_str());
  }
  file.close();
  esp_camera_fb_return(fb); // Clear memory
}
