# SMART Workspace Monitoring System

An IoT-based smart desk that utilizes multi-sensor data and real-time analysis to detect and provide feedback on user workspace behavior.

## Table of Contents

- [Overview](#overview)  

- [Hardware Components](#hardware-components)  

- [Software and Dependencies](#software-and-dependencies)  

- [Usage](#usage)  

- [Results and Demonstration](#results-and-demonstration)  

## Overview

Describe the objective of your project, the problem it solves, and the main features.

The SMART Workspace Monitoring System is designed to improve user productivity and focus while seated at a desk. The system monitors user behavior across three primary modes: studying, gaming, and eating. It then applies real-time sensor data analysis to interpret user actions within their context/

By transforming a standard desk into a sensor-enabled device, the system collects data from multiple souces, including weight distribution, proximity, and RFID-based object identification. The data is used to infer user activity and posture, and then presented through a live web dashboard.

Main Features:
- RFID-based activity mode detection using the MFRC522 reader with tagged desk objects
- Load cell based weight sensing for tracking object placement and arm pressure distribution
- HC-SR04 ultrasonic proximity sensing for posture and presence detection
- Context-aware behavior analysis using time-thresholded rules 
- LED strip feedback system with mode-based color mapping 
- MQTT-based communication pipeline between ESP32 nodes and Raspberry Pi broker
- Django-based web dashboard with live data streaming and visualization


## Hardware Components

| Component | Quantity | Purpose |
|---|---:|---|
| Raspberry Pi 3B+ | 1 | Runs the MQTT local broker, web dashboard, and MQTT listener scripts. |
| ESP32 | 2 | Collects sensors data, turns on LEDs, and publishes to the MQTT topics. |
| MFRC522 RFID reader | 1 | Detects RFID tags for activity mode selection. |
| RFID Tags | 3 | Represent different desk usage modes. |
| Load cell + HX711 | 2 | Measures weight distribution and object interaction on the desk. |
| HC-SR04 Ultrasonic sensor | 1 |  Measures user distance for posture and presence detection. |
| RGB LED Strip | 1 | Provides visual feedback based on mode and user behavior. |
| Standing Tray Table | 1  | Portable physical platform to install the sensors on. |


## Software and Dependencies

Mention the programming language and libraries used:

Languages: Python, C, C++, JavaScript, HTML, CSS

Libraries:
- For ESP32
    - Adafruit NeoPixel by Adafruit 
    - PubSubCient by Nick O'Leary
    - Arduino by Arduino
    - HX711 Arduino Library by Bogdan Necula
- For Raspberry Pi:
    - Django 
    - Paho-mqtt

For the ESP32, download all libraries using the Arduino IDE library manager.

For the Raspberry Pi, first create a virtual environment. Activate, and install all the dependencies with:

```bash
pip install -r requirements.txt  
```

## Usage

For the Raspberry Pi:
Before running anything, make sure the mqtt broker is installed on the raspberry pi. This is done with:
```bash
sudo apt install mosquitto mosquitto-clients 
```
1. Create a terminal instance and open the folder that the project is downloaded in. Then open the main folder.
```bash
Cd dashboard
```
2. Run your virtual environment 
3. Update the database with
```bash
Python3 manage.py makemigrations
Python3 manage.py migrate
```
Then run the project with
```bash
Python3 manage.py runserver 0.0.0.0:8000
```

The terminal will print the address of the server, put that in the browser with :8000 for the port of the program:
Ex: 127.0.0.1:8000

4. Before running the mqtt listeners, please change the address that the listeners will connect to in the mqtt.py file. This should be the ip address of the raspberry pi, found by running ``hostname -I``.

5. Then run the two mqtt listener scripts by creating two new terminal instances.
Follow steps 2 and 3.
Then run the listener with your own topic
Ex: ``Python3 mqtt.py -t huanga9/weight``
Repeat steps 12 -16 for the second listener

For the ESP32:
1. Before running anything, go into the wifi_mqtt.cpp / .ino file, and change the address of the mqtt broker. This should be the same as the raspberry pi above.
2. Also you should make sure the topic name matches the topic that will be listened to on the raspberry pi.
3. Lastly, add a file known as secrets.h into both directories. In this file, define the user and password for your rcsid, as this will run on the rpi_wpa2 network.
4. Once that is done, simply open the ino file and flash the code. It will run automatically.
Repeat the above steps for the other esp32.


## Results and Demonstration 
The SMART Workspace Monitoring System was successfully implemented as a fully integrated IoT system, demonstrating reliable real-time sensing, communication, and feedback across all components. 

During testing, RFID-tagged objects were used to switch between studying, gaming, and eating modes, with the system correctly detecting each transition and updating both the LED indicators and web dashboard in real time. Sensor data from the load cells and ultrasonic sensor was consistently captured and processed, allowing the system to infer user posture and activity (e.g., leaning forward, sitting upright, or stepping away). The MQTT pipeline between the ESP32 nodes and Raspberry Pi operated with low latency, enabling smooth end-to-end data flow from hardware to visualization. 

The Django-based dashboard effectively displayed live data using a sliding window approach, maintaining readability while updating continuously. Behavior analysis based on time-thresholded rules successfully identified patterns such as poor posture, triggering appropriate visual feedback through the LED system. While some limitations were observed,, such as instability with constant RFID scans and performance constraints under high volume data publishing, the system remained stable under normal operation. Overall, the project demonstrates a practical and extensible approach to combining embedded systems, real-time communication, and data analysis to monitor and improve workspace habits.
