# AVSN Data Logger
# Run this on your Laptop while connected to the ESP32

import serial
import time
import csv
from datetime import datetime

# --- CONFIGURATION ---
SERIAL_PORT = 'COM3'  # REPLACE with your ESP32 Port (e.g., COM3 on Windows, /dev/ttyUSB0 on Mac)
BAUD_RATE = 115200
CSV_FILE = 'AVSN_Security_Log.csv'

# Setup Serial Connection
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"Connected to AVSN System on {SERIAL_PORT}")
except:
    print("ERROR: Could not connect. Check COM port.")
    exit()

# Create the CSV file with headers if it doesn't exist
try:
    with open(CSV_FILE, 'x', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["Date", "Time", "Event", "Sensor_Value"])
except FileExistsError:
    pass # File already exists, we will append to it

print("Listening for Intrusion Events... (Press Ctrl+C to Stop)")

while True:
    try:
        # Read a line from the ESP32
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').strip()
            
            # Check if it's our data format "TRIGGER,2400"
            if "," in line:
                event_type, sensor_value = line.split(",")
                
                # Get current time
                now = datetime.now()
                date_str = now.strftime("%Y-%m-%d")
                time_str = now.strftime("%H:%M:%S")
                
                # Print to console
                print(f"[{time_str}] {event_type} detected! Level: {sensor_value}")
                
                # Save to Excel/CSV
                with open(CSV_FILE, 'a', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow([date_str, time_str, event_type, sensor_value])
                    
            else:
                # Just print debug messages
                print(f"Debug: {line}")
                
    except KeyboardInterrupt:
        print("\nLogger Stopped.")
        ser.close()
        break
    except Exception as e:
        print(f"Error: {e}")
