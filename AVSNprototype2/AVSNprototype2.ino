
// Setup: basic LEDs (Pins 3,5), Buzzer (Pin 8), Sound (A0), I2C LCD

//Download liquidcrystal library 
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// PIN MAPPING 
#define PIN_SOUND   A0
#define PIN_RED_LED 3  
#define PIN_BLUE_LED 5  
#define PIN_BUZZER  8

//LED have a resistor connected in the anode via a microcontroller pin

// --- CONFIGURATION ---
// Sensitivity: Lower this number (e.g., 400) to make it trigger easier. Worked best at 1000 for me(Play around with).
const int SOUND_THRESHOLD = 1000; 
const long ALARM_DURATION = 4000; // How long the alarm lasts (ms), change as you wish. 

// --- OBJECTS & VARIABLES ---
//Must find lcd specifications (my module was 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2); 

bool isAlarmActive = false;
unsigned long alarmStartTime = 0;
unsigned long lastFlashTime = 0;
bool toggleState = false; 

void setup() {
  Serial.begin(115200); // Important: Must match Python baud rate
  
  pinMode(PIN_SOUND, INPUT);
  pinMode(PIN_RED_LED, OUTPUT);
  pinMode(PIN_BLUE_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("AVSN: SYSTEM");
  lcd.setCursor(0, 1);
  lcd.print("ONLINE...");
  delay(2000);
  
  // Ready State
  resetSystem();
}

void loop() {
  int soundLevel = analogRead(PIN_SOUND);

  // 1. TRIGGER LOGIC
  if (soundLevel > SOUND_THRESHOLD && !isAlarmActive) {
    activateAlarm(soundLevel);
  }

  // 2. ALARM LOOP
  if (isAlarmActive) {
    // Flash Lights & Siren logic
    if (millis() - lastFlashTime > 150) { // Flash every 150ms
      lastFlashTime = millis();
      toggleState = !toggleState;
      
      if (toggleState) {
        digitalWrite(PIN_RED_LED, HIGH);
        digitalWrite(PIN_BLUE_LED, LOW);
        tone(PIN_BUZZER, 1000); // High pitch
      } else {
        digitalWrite(PIN_RED_LED, LOW);
        digitalWrite(PIN_BLUE_LED, HIGH);
        tone(PIN_BUZZER, 700);  // Low pitch
      }
    }
    
    // Check Timer
    if (millis() - alarmStartTime > ALARM_DURATION) {
      resetSystem();
    }
  }
}

void activateAlarm(int level) {
  isAlarmActive = true;
  alarmStartTime = millis();
  
  // Update LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("!! INTRUDER !!");
  lcd.setCursor(0, 1);
  lcd.print("Level: "); lcd.print(level);
  
  // SEND MESSAGE TO PYTHON
  Serial.print("TRIGGER,");
  Serial.println(level);
}

void resetSystem() {
  isAlarmActive = false;
  digitalWrite(PIN_RED_LED, LOW);
  digitalWrite(PIN_BLUE_LED, LOW);
  noTone(PIN_BUZZER);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AVSN: ARMED");
  lcd.setCursor(0, 1);
  lcd.print("Scanning...");
  
  Serial.println("RESET,0");
}
