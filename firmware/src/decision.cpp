#include "decision.h"
#include <cstdio>

DecisionEngine::DecisionEngine() : activated(false) {}

DecisionEngine::~DecisionEngine() {}

bool DecisionEngine::begin() {
    // Configures GPIO pins GP14 (Green LED), GP15 (Red LED), GP16 (Buzzer)
    activated = false;
    return true;
}

void DecisionEngine::processPrediction(int pred_class, float target_conf) {
    if (pred_class == CLASS_ID_TARGET && target_conf >= WAKE_WORD_THRESHOLD) {
        activated = true;
    } else {
        activated = false;
    }
    updateActuators(pred_class, target_conf);
}

void DecisionEngine::updateActuators(int pred_class, float target_conf) {
    if (activated) {
        std::printf("[ACTIVATION] WAKE WORD DETECTED! Class: Hero Arise | Conf: %.2f%%\n", target_conf * 100.0f);
        // Hardware Actuators:
        // GP14 (Green LED) -> HIGH
        // GP15 (Red LED)   -> LOW
        // GP16 (Buzzer)    -> 2000Hz alert pulse for 150ms
        // OLED Display     -> "=== WAKE WORD ===\nHERO ARISE!"
    } else {
        std::printf("[IDLE] Listening... Class: %s | Target Conf: %.2f%%\n", 
                    (pred_class == CLASS_ID_UNKNOWN ? "Unknown" : "Noise"), 
                    target_conf * 100.0f);
        // Hardware Actuators:
        // GP14 (Green LED) -> LOW
        // GP15 (Red LED)   -> HIGH
        // GP16 (Buzzer)    -> Off
        // OLED Display     -> "SIH2672 Activator\nListening..."
    }
}

bool DecisionEngine::isActivated() const {
    return activated;
}
