# Hero Arise — Low-Latency Edge Voice Activator

**SIH 2026 Problem Statement:** SIH26172 (Hardware / Smart Automation)  
**Organization:** Indian Space Research Organisation (ISRO)  
**Team:** BUGGY-BOTS  
**Target Wake Word:** `"Hero Arise"`

---

## Why We Built This

Most modern voice assistants ship your voice straight to a remote cloud server. In mission-critical, remote, or secure environments—like the ones ISRO operates in—that model completely falls apart due to latency, network dropouts, and privacy risks.

**Hero Arise** is built around a simple principle: **intelligence belongs on the silicon.** It's a completely offline, ultra-low-latency voice activator running directly on an ESP32-S3. From pulling audio samples off the digital mic to Mel-frequency feature extraction and INT8 neural inference, everything runs locally on bare metal—zero cloud calls, zero internet dependency.

---

## The Tech Stack

* **Silicon:** ESP32-S3 DevKitC-1 (Dual-Core Tensilica LX7 @ 240 MHz, hardware vector instructions enabled)
* **Neural Architecture:** Custom 1D-CNN with just **4,083 parameters**
* **Quantization:** INT8 post-training quantization (**0.00% accuracy drop** vs. 32-bit float)
* **Audio Pipeline:** INMP441 digital MEMS microphone via hardware I2S DMA (16 kHz mono)
* **Live Telemetry:** SSD1306 0.96" I2C OLED for real-time confidence scores and microsecond-level latency
* **Physical Trigger:** Active Buzzer (GPIO 6), Status LED (GPIO 4), and an external wake interrupt pin

---

## Real-World Benchmarks

We kept the model lean so it leaves plenty of headroom on the MCU for other tasks:

| Metric | What We Achieved | Constraint / Benchmark |
| :--- | :--- | :--- |
| **Test Accuracy** | **86.57%** | $> 80\%$ target |
| **"Hero Arise" Precision** | **92.00%** | Reliable target keyword spotting |
| **Quantization Loss** | **0.00%** | No degradation going from FP32 to INT8 |
| **Inference Latency** | **< 30 ms** | Well under the 100 ms threshold |
| **SRAM Footprint** | **45.9 KB (14%)** | Runs safely within 320 KB internal RAM |
| **Flash Storage** | **553.1 KB (16.6%)** | Lightweight footprint |
| **Tensor Arena** | **20.0 KB** | Statically pre-allocated (zero `malloc`) |

---

## Hardware Hookup Guide

Wiring mistakes are easy to make on dense DevKits. Use this exact pinout from `firmware/include/config.h`:

| Component | Pin | ESP32-S3 GPIO | Purpose |
| :--- | :--- | :--- | :--- |
| **SSD1306 OLED** | VCC / GND | **3.3V / GND** | 3.3V rail only (do not use 5V) |
| | SDA / SCL | **GPIO 8 / GPIO 9** | Hardware I2C data & clock |
| **INMP441 Mic** | VDD / GND | **3.3V / GND** | Digital power rail |
| | L/R | **GND** | Tied to GND for Left Channel audio |
| | SCK / WS / SD | **GPIO 12 / 11 / 10** | Continuous bit clock, word select, data line |
| **Active Buzzer** | (+) / (-) | **GPIO 6 / GND** | 120 ms acoustic confirmation pulse |
| **Status LED** | Anode / Cathode | **GPIO 4 / GND** | Visual confirmation (via 220Ω resistor) |

---

## Project Layout

```text
Voice_Activator_Project/
├── firmware/
│   ├── include/
│   │   ├── config.h             # Pin assignments, thresholds, and tensor memory caps
│   │   └── model_data.h         # Quantized INT8 weights C array (13.2 KB)
│   └── src/
│       └── main.cpp             # DMA sampling, TFLM inference, and OLED telemetry
├── ml/
│   ├── results/                 # Parity reports, confusion matrices, and audit logs
│   └── train_model.py           # Model training and INT8 quantization script
├── scripts/
│   ├── extract_mfe.py           # Mel-frequency energy feature extraction reference
│   └── verify_python_cpp.py     # Python vs. C++ parity check (100% verified)
├── platformio.ini               # PlatformIO toolchain configuration
└── README.md

Team BUGGY-BOTS
Aathira Venuraj

Shrinivas Hari

Jero A

Reshma V

Sujatha K

Akzhara Baskar
