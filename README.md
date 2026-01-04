
# AVSN: Audio-Visual Security Node

## Project Overview

The **AVSN (Audio-Visual Security Node)** is an autonomous intrusion detection system designed to secure physical environments through multi-modal sensor fusion. This collaborative engineering initiative integrates embedded hardware, analog circuit design, and computer vision software to detect acoustic anomalies and capture real-time photographic evidence of unauthorized access.

This project simulates a real-world R&D environment by analyzing and implementing two distinct architectural approaches: a high-power IoT solution (Prototype I) and a logic-level sensor fusion bridge (Prototype II). The system demonstrates proficiency in full-stack engineering—ranging from **PCB design (KiCad)** and **mechanical modeling (AutoCAD)** to **C++ firmware development** and **Python-based signal processing**.

## Team & Collaboration

* **3rd Year Electrical Engineering (TMU):** Rijan Subedi 
* **3rd Year Computer Engineering (McMaster University):** Afraz Akram 

*This project emphasizes cross-disciplinary collaboration, version control management, and agile hardware iteration.*

## Technical Stack & Tools

**Software & Firmware**

* **Arduino IDE (C++):** Firmware development for sensor polling and actuator control.
* **Visual Studio Code (Python):** Backend development for serial communication and computer vision logic.
* **OpenCV (`cv2`):** Real-time image processing and frame capture.
* **KiCad:** Schematics capture and PCB layout design.
* **AutoCAD:** 3D modeling of mechanical enclosures and component housing.

**Hardware & Components**

* **Microcontrollers:** ESP32 (Prototype I), Arduino Uno R3 (Prototype II), ESP32-CAM.
* **Analog Electronics:** NPN BJT Transistors (2N3904), Discrete Darlington Pairs, Flyback Diodes (1N4148).
* **Sensors & Actuators:** KY-038 Electret Microphone, I2C LCD (16x2), Piezo Buzzers, High-Output LEDs.

## Prototype I: High-Power ESP32 Architecture

*Focus: IoT Integration & Analog Switching Theory*

Prototype I was designed as a standalone IoT unit powered by the **ESP32 microcontroller**. The primary engineering objective was to drive high-current external loads (such as floodlights or high-decibel sirens) using a 9V power source, isolated from the 3.3V logic rail.

### Theoretical Framework & Circuit Analysis

The core of this design utilizes a **Discrete Darlington Pair configuration** using dual 2N3904 NPN transistors.

* **Current Gain ():** By cascading two transistors, the system achieves a total current gain of . This allows the low-current GPIO of the ESP32 to drive heavy loads without thermal stress.
* **Inductive Load Protection:** To prevent voltage spikes from destroying the transistors during switching events, **1N4148 Flyback Diodes** were implemented across the load terminals to provide a safe dissipation path for Back-EMF.

### PCB Design (KiCad)

The PCB for Prototype I focuses on power isolation and thermal management.

* **Trace Width Calculation:** Power traces for the 9V rail were widened to 20 mil to accommodate higher current density.
* **Ground Planes:** A split ground plane strategy was employed to separate the noisy high-power return path from the sensitive ESP32 RF ground.

### Mechanical Design (AutoCAD)

* **Enclosure:** Designed a vented chassis to allow for passive cooling of the transistor array.
* **Mounting:** Custom standoffs modeled for the ESP32 Dev Module form factor.

## Prototype II: Arduino Sensor Fusion Bridge

*Focus: Serial Communication, Logic Integration & Reliability*

Prototype II shifts focus toward robust serial communication and sensor fusion. It utilizes an **Arduino Uno** as the primary sensory node, offloading heavy video processing to a host machine via Python. This architecture prioritizes reliability and modularity.

### System Theory & Logic

This prototype employs a "Dual-System" architecture:

1. **The Sensory Node (Arduino):** Performs continuous ADC sampling of the acoustic environment. It utilizes a software-defined threshold algorithm to filter background noise. Upon triggering (Value > 1000), it activates a local alert loop and transmits a `TRIGGER` interrupt string over USB Serial.
2. **The Vision Node (Python/ESP32-CAM):** A Python script listens for the serial interrupt. Synchronization is handled via the `pyserial` library. Upon receipt of the trigger, the script accesses the IP video stream from a separate ESP32-CAM and executes an **OpenCV** capture command to log the event.

### Electrical Schematic

The design features a direct-drive logic circuit with critical safety implementations:

* **Current Limiting:** 220$\Omega$ resistors are placed in series with visual indicators to limit current draw to safe GPIO levels (<20mA).
* **Pull-Down Resistors:** Integrated on sensor inputs to prevent floating voltage states and false alarms.

### PCB Design (KiCad)

The PCB for Prototype II is designed as a custom **Arduino Uno Shield**.

* **Form Factor:** Matches the standardized Arduino R3 header footprint for a "plug-and-play" stackable design.
* **Signal Integrity:** Analog sensor traces (A0) are routed away from the high-frequency digital switching lines (SDA/SCL) to minimize crosstalk and signal noise.

### Mechanical Design (AutoCAD)

* **Camera Mount:** A specialized gimbal mount was modeled in AutoCAD to hold the ESP32-CAM at a 45-degree angle, optimizing the Field of View (FOV) for facial capture.
* **Integration:** The enclosure features external access ports for the USB serial bridge, allowing for seamless debugging and reprogramming.

## Comparative Analysis

**Prototype I Strengths:**

* **Wireless Capability:** Native WiFi integration via ESP32 allows for completely wireless deployment.
* **Load Capacity:** The Darlington Pair configuration enables the control of industrial-grade deterrents.

**Prototype II Strengths:**

* **Processing Power:** Offloading image processing to a host PC allows for advanced computer vision features (e.g., face detection) without lagging the microcontroller.
* **Stability:** The 5V logic level of the Arduino Uno provides superior noise immunity for analog sensors compared to the 3.3V ESP32.

## Installation & Usage

### Prerequisites

* Python 3.9+
* `pip install opencv-python pyserial`

### Setup Instructions

1. **Hardware Assembly:** Mount the custom PCB shield onto the Arduino Uno headers.
2. **Firmware:**
* Flash `AVSN_Controller.ino` to the Arduino Uno.
* Flash `CameraWebServer` to the ESP32-CAM.


3. **Software:**
* Open `main.py` in Visual Studio Code.
* Update `ARDUINO_PORT` and `CAM_URL` to match your local configuration.
* Run the script: `python main.py`



## Conclusion

The AVSN project successfully demonstrates the integration of hardware and software to solve real-world security challenges. By evaluating both the high-power analog approach of Prototype I and the integrated logic approach of Prototype II, the team gained comprehensive experience in circuit protection, PCB fabrication workflows, and serial communication protocols. Future iterations will focus on merging these designs to create a standalone, battery-operated device with on-board AI processing.
