# Mochi 🤖

Mochi is a low-cost autonomous desktop companion robot designed to exhibit lifelike behaviors through computer vision, expressive animations, and autonomous decision making.

The project combines embedded systems, computer vision, and robotics to create an interactive robot capable of recognizing people, tracking faces, navigating its environment, and expressing emotions through an animated OLED display.

---

# Current Progress

### Vision
- ✅ ESP32-CAM live video streaming
- ✅ OpenCV vision pipeline
- ✅ MediaPipe face detection
- 🔄 Face center extraction
- ⏳ Face tracking
- ⏳ Multi-face tracking

### Robot Control
- ✅ Differential drive locomotion
- ✅ Motor driver integration
- ✅ Finite State Machine (FSM)
- 🔄 Face-directed turning
- ⏳ Autonomous human following

### Personality
- ✅ Animated OLED eyes
- ✅ Emotion system
- 🔄 Behavior refinement
- ⏳ Interactive personality

---

# Hardware

## Controller
- ESP32 DevKit
- ESP32-CAM

## Motor System
- TB6612FNG Motor Driver
- 2× N20 DC Gear Motors

## Power System
- 3.7V Li-Po Battery
- LMR51420 Buck Converter

## Sensors
- Time-of-Flight Distance Sensor
- IR Edge Detection Sensors
- IMU (planned/integration)

---

# Software

- Arduino Framework (C++)
- Python
- OpenCV
- MediaPipe
- Finite State Machine (FSM)
- Computer Vision Pipeline

---

# Project Goals

Mochi aims to become an expressive desktop robot capable of:

- Human detection
- Face tracking
- Autonomous following
- Obstacle avoidance
- Emotion-driven behaviors
- Gesture recognition
- Voice interaction
- AI-powered companion features

---

# Roadmap

## Vision
- [x] ESP32-CAM streaming
- [x] OpenCV camera pipeline
- [x] MediaPipe face detection
- [ ] Face tracking
- [ ] Face recognition
- [ ] Gesture recognition

## Navigation
- [x] Differential drive
- [ ] Face following
- [ ] Obstacle avoidance
- [ ] Autonomous exploration

## Interaction
- [x] Animated eyes
- [x] Emotion system
- [ ] Voice interaction
- [ ] Interactive personality

---

# Long-Term Vision

Mochi is intended to serve as:

- A personal robotics platform for experimentation
- A portfolio project
- The basis for undergraduate research
- A foundation for future ROS2 and Jetson Orin integration
- A potential research paper on low-cost autonomous companion robotics
