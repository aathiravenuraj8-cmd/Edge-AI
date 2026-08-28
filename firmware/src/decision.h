#ifndef DECISION_H
#define DECISION_H

#include "config.h"

// Decision Engine & Peripheral Actuator Controller
class DecisionEngine {
public:
    DecisionEngine();
    ~DecisionEngine();

    // Initializes GPIO actuator pins (LED Green, LED Red, Buzzer) and OLED display
    bool begin();

    // Evaluates model probabilities against detection threshold and updates hardware actuators
    void processPrediction(int pred_class, float target_conf);

    // Updates physical OLED display, Green/Red status LEDs, and Piezo Buzzer
    void updateActuators(int pred_class, float target_conf);

    // Getter for wake word status
    bool isActivated() const;

private:
    bool activated;
};

#endif // DECISION_H
