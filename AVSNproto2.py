import cv2
import serial
import time
import threading
import datetime
import os

# 1. Your Arduino Port (e.g., 'COM3' on Windows
ARDUINO_PORT = 'COM3' 

# 2. Your ESP32-CAM IP Address
# IMPORTANT: Keep the ":81/stream" at the end!
CAM_URL = 'http://10.0.0.181:81/stream' 

# -------------------------------------------------

print("--- AVSN SECURITY SYSTEM INITIALIZING ---")

# 1. CONNECT TO ARDUINO
try:
    arduino = serial.Serial(ARDUINO_PORT, 115200, timeout=1)
    time.sleep(2) # Give Arduino time to reset
    print(f"[SUCCESS] Connected to Arduino on {ARDUINO_PORT}")
except Exception as e:
    print(f"[ERROR] Could not connect to Arduino: {e}")
    print("Check your cable and make sure the Port is correct!")
    exit()

# 2. CONNECT TO CAMERA
print(f"[INFO] Connecting to Camera at {CAM_URL}...")
cap = cv2.VideoCapture(CAM_URL)

if not cap.isOpened():
    print("[ERROR] Could not connect to ESP32-CAM stream.")
    print("Tips: 1. Check IP address. 2. Close browser tabs. 3. Reset the Camera.")
    # We continue so you can still see Arduino data, but video won't work

# GLOBAL VARIABLES
latest_command = ""
text_display_end_time = 0  # Timer for keeping text on screen

# --- HELPER: BACKGROUND LISTENER ---
def read_serial():
    """Listens for messages from Arduino (runs in background)"""
    global latest_command
    while True:
        try:
            if arduino.in_waiting > 0:
                line = arduino.readline().decode('utf-8').strip()
                if line:
                    latest_command = line
                    print(f"[ARDUINO SAYS]: {line}")
        except:
            pass

# Start the listener thread
serial_thread = threading.Thread(target=read_serial, daemon=True)
serial_thread.start()

# --- MAIN SECURITY LOOP ---
print("[SYSTEM READY] Press 'q' inside the video window to quit.")

while True:
    # 1. Grab a frame from the camera
    ret, frame = cap.read()
    
    if ret:
        # Resize to make it run faster on your laptop
        frame = cv2.resize(frame, (640, 480))
        current_time = time.time()

        # 2. CHECK FOR TRIGGER (From Arduino)
        if "TRIGGER" in latest_command:
            print("[ALERT] Intruders detected! Capturing evidence...")
            
            # Set the timer: Show text for 3 seconds from NOW
            text_display_end_time = current_time + 3.0
            
            # --- SNAPSHOT LOGIC ---
            timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = f"intruder_{timestamp}.jpg"
            
            # Draw text ON THE PHOTO explicitly before saving
            cv2.putText(frame, "!! INTRUDER DETECTED !!", (50, 50), 
                        cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 3)
            
            # Save the file to disk
            success = cv2.imwrite(filename, frame)
            
            if success:
                # Print the EXACT location so you can find it
                full_path = os.path.abspath(filename)
                print(f"[EVIDENCE SAVED] Location: {full_path}")
            else:
                print("[ERROR] Failed to save photo.")
            
            # Reset command so we don't spam photos
            latest_command = ""

        # 3. DRAW PERSISTENT TEXT (If timer is active)
        if current_time < text_display_end_time:
            cv2.putText(frame, "!! INTRUDER DETECTED !!", (50, 50), 
                        cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 3)

        # 4. Show the video feed
        cv2.imshow("AVSN Live Feed", frame)

    # Quit logic (Press 'q')
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# --- CLEANUP ---
cap.release()
cv2.destroyAllWindows()
arduino.close()
print("System Shutdown.")
