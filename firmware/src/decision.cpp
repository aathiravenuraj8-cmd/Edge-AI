#include "decision.h"
#include <cstdio>
#include "pico/stdlib.h"
#include "../include/config.h"

DecisionEngine::DecisionEngine() 
    : activated(false), consecutive_hits(0), cooldown_until_ms(0) {}

DecisionEngine::~DecisionEngine() {}

bool DecisionEngine::begin() {
    activated = false;
    consecutive_hits = 0;
    cooldown_until_ms = 0;

    // Initialize LED and Buzzer pins as outputs
    gpio_init(PIN_LED_GREEN);
    gpio_set_dir(PIN_LED_GREEN, GPIO_OUT);
    gpio_init(PIN_LED_RED);
    gpio_set_dir(PIN_LED_RED, GPIO_OUT);
    gpio_init(PIN_BUZZER);
    gpio_set_dir(PIN_BUZZER, GPIO_OUT);

    // Set initial state: Red LED on (listening), Green LED and buzzer off
    gpio_put(PIN_LED_GREEN, 0);
    gpio_put(PIN_LED_RED, 1);
    gpio_put(PIN_BUZZER, 0);

    return true;
}

void DecisionEngine::processPrediction(int pred_class, float target_conf, uint32_t current_time_ms, bool vad_speech) {
    // 1. Cooldown Check
    if (current_time_ms > 0 && current_time_ms < cooldown_until_ms) {
        consecutive_hits = 0;
        activated = false;
        std::printf("[COOLDOWN] Ignoring wake-word detections (Cooldown active until %u ms)\n", cooldown_until_ms);
        updateActuators(pred_class, target_conf);
        return;
    }

    // 2. VAD Speech + Target Class + Threshold Check
    bool valid_hit = vad_speech && (pred_class == CLASS_ID_TARGET) && (target_conf >= WAKE_WORD_THRESHOLD);

    if (valid_hit) {
        consecutive_hits++;
        std::printf("[CONFIRMATION] Wake confirmation: %d/%d (Conf: %.2f%%)\n", 
                    consecutive_hits, CONFIRMATION_COUNT, target_conf * 100.0f);
        
        if (consecutive_hits >= CONFIRMATION_COUNT) {
            activated = true;
            if (current_time_ms > 0) {
                cooldown_until_ms = current_time_ms + COOLDOWN_MS;
            }
            consecutive_hits = 0;
            std::printf("[ACTIVATION] WAKE CONFIRMED! Triggering action. 1500 ms Cooldown active.\n");
        } else {
            activated = false;
        }
    } else {
        // Interruption: confidence below threshold or non-speech -> Reset counter
        consecutive_hits = 0;
        activated = false;
    }

    updateActuators(pred_class, target_conf);
}

void DecisionEngine::updateActuators(int pred_class, float target_conf) {
    if (activated) {
        std::printf("[ACTIVATION] WAKE WORD DETECTED! Class: Hero Arise | Conf: %.2f%%\n", target_conf * 100.0f);
        gpio_put(PIN_LED_GREEN, 1);
        gpio_put(PIN_LED_RED, 0);
        gpio_put(PIN_BUZZER, 1);
    } else if (consecutive_hits > 0) {
        std::printf("[CONFIRMING] Wake confirmation: %d/%d\n", consecutive_hits, CONFIRMATION_COUNT);
        gpio_put(PIN_LED_GREEN, 0);
        gpio_put(PIN_LED_RED, 1);
        gpio_put(PIN_BUZZER, 0);
    } else {
        std::printf("[IDLE] Listening... Class: %s | Target Conf: %.2f%%\n", 
                    (pred_class == CLASS_ID_UNKNOWN ? "Unknown" : "Noise"), 
                    target_conf * 100.0f);
        gpio_put(PIN_LED_GREEN, 0);
        gpio_put(PIN_LED_RED, 1);
        gpio_put(PIN_BUZZER, 0);
    }
}

bool DecisionEngine::isActivated() const {
    return activated;
}

int DecisionEngine::getConsecutiveHits() const {
    return consecutive_hits;
}

bool DecisionEngine::isInCooldown(uint32_t current_time_ms) const {
    return current_time_ms > 0 && current_time_ms < cooldown_until_ms;
}
