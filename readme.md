# Gesture Controller Boat

A wireless gesture-controlled boat built using two ESP32 boards, an LSM6DSO accelerometer, and ESP-NOW communication. The boat moves according to the hand gestures made with the controller.

## Project Overview

This project consists of two parts:

### Transmitter (Controller)
- ESP32 Development Board
- LSM6DSO Accelerometer
- Reads hand tilt
- Sends movement commands wirelessly using ESP-NOW

### Receiver (Boat)
- ESP32 Development Board
- Motor Driver Module
- DC Motors
- Receives commands via ESP-NOW
- Controls the boat's movement

---

## Features

- Wireless communication using ESP-NOW
- Real-time gesture recognition
- Low-latency communication
- No Wi-Fi router required
- Forward
- Backward
- Left
- Right
- Stop

---

## Hardware Used

### Controller
- ESP32
- LSM6DSO Accelerometer
- Battery

### Boat
- ESP32
- Motor Driver (L298N/TB6612FNG or compatible)
- DC Motors
- Battery
- Boat Chassis

---

# Hardware Connections

## Controller (ESP32 + LSM6DSO)

| LSM6DSO Pin | ESP32 Pin |
|-------------|-----------|
| VIN / VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |
| CS | 3.3V (I²C Mode) |
| SDO / SA0 | 3.3V *(I²C Address = 0x6B)* |

> **Note:** If SA0 is connected to GND, the I²C address becomes **0x6A**. This project uses **0x6B**.

---

## Boat (ESP32 + Motor Driver)

### ESP32 to Motor Driver

| ESP32 Pin | Motor Driver Pin |
|-----------|------------------|
| GPIO 25 | IN1 |
| GPIO 26 | IN2 |
| GPIO 27 | IN3 |
| GPIO 14 | IN4 |
| 5V / VIN | VCC |
| GND | GND |

### Motor Driver to Motors

| Motor Driver | Motor |
|--------------|-------|
| OUT1 | Left Motor (+) |
| OUT2 | Left Motor (-) |
| OUT3 | Right Motor (+) |
| OUT4 | Right Motor (-) |

### Power Connections

| Component | Power |
|-----------|-------|
| ESP32 | Battery / USB |
| Motor Driver | External Battery |
| All Grounds | Common GND |

> **Important:** The ESP32 and Motor Driver **must share a common ground (GND)** for proper operation.

---

## Working Principle

1. The LSM6DSO measures the controller's orientation.
2. The ESP32 processes the accelerometer data.
3. A movement command is generated.
4. The command is transmitted wirelessly using ESP-NOW.
5. The receiver ESP32 receives the command.
6. The motor driver drives the DC motors.
7. The boat moves according to the detected gesture.

---

## Gesture Mapping

| Hand Gesture | Boat Action |
|--------------|-------------|
| Tilt Forward | Forward |
| Tilt Backward | Backward |
| Tilt Left | Left |
| Tilt Right | Right |
| Keep Flat | Stop |

---

## Project Structure

```
Gesture-controller-boat-v1/
│
├── transmitter-gcb-2.0/
│   └── Transmitter source code
│
├── receiver-gcb-2.0/
│   └── Receiver source code
│
└── README.md
```

---

## Future Improvements

- Adjustable gesture sensitivity
- Speed control using PWM
- Battery level monitoring
- Waterproof enclosure
- OLED status display
- PID-based motion control
- Obstacle avoidance
- Gesture calibration mode

---

## Author

**Ashutosh Sahu**

B.Tech Electronics & Telecommunication Engineering

Interested in:
- Embedded Systems
- IoT
- Robotics
- ESP32 Development

---

## License

This project is released under the MIT License.