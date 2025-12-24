/*
 * Project: AVSN (Audio-Visual Security Node) - Master Controller
 * Author: [Your Names]
 * Hardware: ESP32 Dev Board, KY-038, I2C LCD, Discrete Darlington Pairs
 */

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// --- PIN DEFINITIONS (Matches our Schematic) ---
#define PIN_SOUND_ANALOG  34  // KY-038 Analog Out
#define PIN_RED_LIGHT     25  // Red Darlington Base
#define PIN_BLUE_LIGHT    26  // Blue Darlington Base
#define PIN_CAM_TRIGGER   13  // Signal to ESP32-CAM
#define PIN_SDA           21  // I2C Data
#define PIN_SCL           22  // I2C Clock

// --- CONFIGURATION ---
const int SOUND_THRESHOLD = 2000; // Adjust after testing (0-4095)
const long ALARM_DURATION = 5000; // How long the alarm runs (ms)
const long FLASH_SPEED = 100;     // Speed of police lights (ms)

// --- VARIABLES ---
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set I2C address to 0x27, 16 chars, 2 lines
bool isAlarmActive = false;
unsigned long alarmStartTime = 0;
unsigned long lastFlashTime = 0;
bool toggleLight = false; // Used to flip between Red and Blue

void setup() {
  Serial.begin(115200); // Fast communication for Python
  
  // Pin Modes
  pinMode(PIN_SOUND_ANALOG, INPUT);
  pinMode(PIN_RED_LIGHT, OUTPUT);
  pinMode(PIN_BLUE_LIGHT, OUTPUT);
  pinMode(PIN_CAM_TRIGGER, OUTPUT);
  
  // Initialize Outputs
  digitalWrite(PIN_RED_LIGHT, LOW);
  digitalWrite(PIN_BLUE_LIGHT, LOW);
  digitalWrite(PIN_CAM_TRIGGER, LOW);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("AVSN: ARMED");
  lcd.setCursor(0, 1);
  lcd.print("Status: Secure");
}

void loop() {
  // 1. READ SENSORS
  int soundLevel = analogRead(PIN_SOUND_ANALOG);

  // 2. CHECK TRIGGER (Only if alarm isn't already running)
  if (soundLevel > SOUND_THRESHOLD && !isAlarmActive) {
    activateAlarm(soundLevel);
  }

  // 3. HANDLE ALARM STATE (Non-blocking logic)
  if (isAlarmActive) {
    handleLights(); // Flash lights
    
    // Check if alarm time is over
    if (millis() - alarmStartTime > ALARM_DURATION) {
      stopAlarm();
    }
  }

  // Small delay to stabilize ADC reading (optional)
  delay(10); 
}

// --- HELPER FUNCTIONS ---

void activateAlarm(int level) {
  isAlarmActive = true;
  alarmStartTime = millis();
  
  // Trigger Camera (Send a 100ms HIGH pulse)
  digitalWrite(PIN_CAM_TRIGGER, HIGH);
  delay(50); // Short blocking delay is okay here to ensure camera sees it
  digitalWrite(PIN_CAM_TRIGGER, LOW);

  // Update LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("!! INTRUDER !!");
  lcd.setCursor(0, 1);
  lcd.print("Lvl: "); lcd.print(level);

  // Send Data to Python (Format: EVENT, VALUE)
  Serial.print("TRIGGER,");
  Serial.println(level);
}

void stopAlarm() {
  isAlarmActive = false;
  
  // Turn off all lights
  digitalWrite(PIN_RED_LIGHT, LOW);
  digitalWrite(PIN_BLUE_LIGHT, LOW);
  
  // Reset LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AVSN: ARMED");
  lcd.setCursor(0, 1);
  lcd.print("Status: Secure");

  // Send Data to Python
  Serial.println("RESET,0");
}

void handleLights() {
  // This uses millis() to flash lights without stopping the code
  if (millis() - lastFlashTime > FLASH_SPEED) {
    lastFlashTime = millis();
    toggleLight = !toggleLight; // Flip state
    
    if (toggleLight) {
      digitalWrite(PIN_RED_LIGHT, HIGH);
      digitalWrite(PIN_BLUE_LIGHT, LOW);
    } else {
      digitalWrite(PIN_RED_LIGHT, LOW);
      digitalWrite(PIN_BLUE_LIGHT, HIGH);
    }
  }
}
