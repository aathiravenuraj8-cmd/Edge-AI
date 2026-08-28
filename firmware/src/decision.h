#ifndef DECISION_H
#define DECISION_H

#include "config.h"
#include <cstdint>

#define CONFIRMATION_COUNT 3
#define COOLDOWN_MS 1500

// Decision Engine & Peripheral Actuator Controller
class DecisionEngine {
public:
    DecisionEngine();
    ~DecisionEngine();

    // Initializes GPIO actuator pins (LED Green, LED Red, Buzzer) and OLED display
    bool begin();

    // Evaluates model probabilities against detection threshold, temporal confirmation, and cooldown
    void processPrediction(int pred_class, float target_conf, uint32_t current_time_ms = 0, bool vad_speech = true);

    // Updates physical OLED display, Green/Red status LEDs, and Piezo Buzzer
    void updateActuators(int pred_class, float target_conf);

    // Getters for status telemetry
    bool isActivated() const;
    int getConsecutiveHits() const;
    bool isInCooldown(uint32_t current_time_ms = 0) const;

private:
    bool activated;
    int consecutive_hits;
    uint32_t cooldown_until_ms;
};

#endif // DECISION_H
