#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "config.h"
#include "model_runner.h"
#include "test_vectors.h"

// Instantiate Adafruit OLED Display
Adafruit_SSD1306 display(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &Wire, -1);

// Instantiate Real INT8 TFLite Model Runner
ModelRunner modelRunner;

void setupOLED() {
    Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
        Serial.println(F("[OLED] ERROR: SSD1306 allocation failed!"));
        return;
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("SIH2672 HERO ARISE"));
    display.println(F("TinyML Activator"));
    display.println(F("Initializing..."));
    display.display();
}

void updateActuators(int pred_class, float target_confidence) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);

    if (pred_class == CLASS_ID_TARGET && target_confidence >= WAKE_WORD_THRESHOLD) {
        // --- WAKE WORD DETECTED ("Hero Arise") ---
        digitalWrite(PIN_LED_GREEN, HIGH);
        digitalWrite(PIN_LED_RED, LOW);

        // Sound brief 2000Hz alert beep on buzzer
        tone(PIN_BUZZER, 2000, 150);

        // Display Wake-Word Notification
        display.println(F("=== WAKE WORD ==="));
        display.setTextSize(2);
        display.println(F("HERO ARISE!"));
        display.setTextSize(1);
        display.printf("Conf: %.1f%%\n", target_confidence * 100.0f);
        display.println(F("STATUS: ACTIVATED"));

        Serial.printf("[ACTIVATION] WAKE WORD DETECTED! Class: Hero Arise | Conf: %.2f%%\n", target_confidence * 100.0f);
    } else {
        // --- NO WAKE WORD DETECTED (Unknown / Noise) ---
        digitalWrite(PIN_LED_GREEN, LOW);
        digitalWrite(PIN_LED_RED, HIGH);
        noTone(PIN_BUZZER);

        display.println(F("SIH2672 Activator"));
        display.setTextSize(1);
        display.println(F("Listening..."));
        
        if (pred_class == CLASS_ID_UNKNOWN) {
            display.println(F("Detected: Unknown"));
        } else {
            display.println(F("Detected: Noise"));
        }
        display.printf("Conf: %.1f%%\n", target_confidence * 100.0f);

        Serial.printf("[IDLE] Listening... Class: %s | Target Conf: %.2f%%\n", 
                      (pred_class == CLASS_ID_UNKNOWN ? "Unknown" : "Noise"), 
                      target_confidence * 100.0f);
    }

    display.display();
}

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 2000);

    Serial.println(F("\n=========================================="));
    Serial.println(F("SIH2672 — HERO ARISE TINYML ACTIVATOR"));
    Serial.println(F("Target MCU: ESP32-S3 | Runtime: TFLite Micro"));
    Serial.println(F("=========================================="));

    // Configure GPIO Pins
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);

    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_RED, HIGH);

    // Initialize OLED Display
    setupOLED();
    delay(1000);

    // Initialize Real TFLite Micro Model
    if (!modelRunner.begin()) {
        Serial.println(F("[FATAL] TFLite Model Initialization Failed!"));
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println(F("MODEL INIT ERROR"));
        display.display();
        while (1) { delay(1000); }
    }

    Serial.println(F("[SYSTEM] Real INT8 Model Ready. Starting Test Mode..."));
}

void loop() {
    // Deterministic Embedded Test Mode: Iterate through precomputed INT8 test vectors
    for (int i = 0; i < NUM_TEST_VECTORS; i++) {
        const TestVector& vec = TEST_VECTORS[i];

        Serial.println(F("\n------------------------------------------"));
        Serial.printf("[TEST MODE] Sample #%d: Expected '%s' (Class %d)\n", 
                      i + 1, vec.name, vec.expected_class);

        float probabilities[3] = {0.0f, 0.0f, 0.0f};

        // Measure TFLite Micro Invoke latency on ESP32
        unsigned long start_us = micros();
        bool success = modelRunner.predict(vec.features, probabilities);
        unsigned long elapsed_us = micros() - start_us;

        if (!success) {
            Serial.println(F("[ERROR] Model inference failed!"));
            continue;
        }

        int pred_class = modelRunner.getPredictedClass(probabilities);
        float target_conf = modelRunner.getTargetConfidence(probabilities);

        Serial.printf("[TFLite Micro] Latency: %.3f ms (%lu us)\n", elapsed_us / 1000.0f, elapsed_us);
        Serial.printf("[TFLite Micro] Probabilities -> Target: %.4f | Unknown: %.4f | Noise: %.4f\n",
                      probabilities[CLASS_ID_TARGET], probabilities[CLASS_ID_UNKNOWN], probabilities[CLASS_ID_NOISE]);
        Serial.printf("[TEST RESULT] Expected: %d | Predicted: %d | Status: %s\n",
                      vec.expected_class, pred_class, (vec.expected_class == pred_class ? "PASS" : "FAIL"));

        // Drive OLED, LEDs, and Buzzer Actuators with Real Model Predictions
        updateActuators(pred_class, target_conf);

        delay(3000); // Display result for 3 seconds before next sample
    }
}
