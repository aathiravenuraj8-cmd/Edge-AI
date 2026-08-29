#include <cstdio>
#include "pico/stdlib.h"
#include "config.h"
#include "decision.h"

// Instantiate the decision engine for hardware control
static DecisionEngine decisionEngine;

// Precomputed test vectors (simulated feature frames for regression testing)
void loop_test_mode() {
    std::printf("[TEST MODE] Starting test vector regression loop...\n");
    
    while (true) {
        // Simulate processing a test vector where the target wake word ("Hero Arise") is matched
        uint32_t current_time_ms = to_ms_since_boot(get_absolute_time());
        
        // Feed a simulated positive match into the decision engine
        // Parameters: pred_class, target_conf, current_time_ms, vad_speech
        decisionEngine.processPrediction(CLASS_ID_TARGET, 0.95f, current_time_ms, true);
        
        // Wait 2 seconds before the next test cycle
        sleep_ms(2000);
        
        // Simulate an idle / listening state cycle
        current_time_ms = to_ms_since_boot(get_absolute_time());
        decisionEngine.processPrediction(CLASS_ID_UNKNOWN, 0.12f, current_time_ms, false);
        
        sleep_ms(2000);
    }
}

int main() {
    stdio_init_all();
    
    // Brief pause to allow serial connection to establish
    sleep_ms(1500);
    std::printf("\n--- HERO ARISE FIRMWARE INITIALIZING (TEST VECTOR MODE) ---\n");

    // Initialize decision engine (configures LED and Buzzer GPIO pins)
    if (!decisionEngine.begin()) {
        std::printf("[ERROR] Failed to initialize DecisionEngine!\n");
    }

    // Run the automated test vector sequence
    loop_test_mode();

    return 0;
}