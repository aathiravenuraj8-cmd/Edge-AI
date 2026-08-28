#include <cstdio>
#include "../include/config.h"
#include "model_runner.h"
#include "audio_capture.h"
#include "mfe.h"
#include "decision.h"
#include "../test_vectors.h"

// Instantiate Modular Components
static ModelRunner modelRunner;
static AudioCapture audioCapture;
static MFEExtractor mfeExtractor;
static DecisionEngine decisionEngine;

// Timing instrumentation structure for pipeline profiling
struct PipelineLatencyMetrics {
    uint32_t audio_capture_us;
    uint32_t mfe_extraction_us;
    uint32_t inference_us;
    uint32_t decision_us;
    uint32_t total_pipeline_us;
};

void setup() {
    std::printf("\n==========================================\n");
    std::printf("SIH2672 — HERO ARISE TINYML ACTIVATOR\n");
    std::printf("Target MCU: Raspberry Pi Pico 2 W (RP2350)\n");
    std::printf("CPU Architecture: ARM Cortex-M33\n");
    std::printf("Runtime: Direct C++ TensorFlow Lite Micro Engine\n");
    #if (CURRENT_RUN_MODE == RUN_MODE_LIVE_MIC)
        std::printf("Operational Mode: LIVE INMP441 I2S MICROPHONE [HARDWARE VALIDATION PENDING]\n");
    #else
        std::printf("Operational Mode: DETERMINISTIC TEST VECTOR REGRESSION MODE\n");
    #endif
    std::printf("==========================================\n");

    // Initialize Hardware Components
    audioCapture.begin();
    decisionEngine.begin();

    // Initialize Real C++ TFLite Micro Model Runner
    if (!modelRunner.begin()) {
        std::printf("[FATAL] TFLite Micro Model Initialization Failed!\n");
        return;
    }

    std::printf("[SYSTEM] Real INT8 Model Ready. Pipeline Active...\n");
}

void loop_test_mode() {
    // Deterministic Embedded Test Mode: Iterate through precomputed INT8 test vectors
    for (int i = 0; i < NUM_TEST_VECTORS; i++) {
        const TestVector& vec = TEST_VECTORS[i];

        std::printf("\n------------------------------------------\n");
        std::printf("[TEST MODE] Sample #%d: Expected '%s' (Class %d)\n", 
                    i + 1, vec.name, vec.expected_class);

        float probabilities[3] = {0.0f, 0.0f, 0.0f};
        PipelineLatencyMetrics metrics = {0, 0, 0, 0, 0};

        // Stage D: Execute INT8 TFLite Micro inference
        bool success = modelRunner.predict(vec.features, probabilities);
        if (!success) {
            std::printf("[ERROR] Model inference failed!\n");
            continue;
        }

        int pred_class = modelRunner.getPredictedClass(probabilities);
        float target_conf = modelRunner.getTargetConfidence(probabilities);

        std::printf("[TFLite Micro] Probabilities -> Target: %.4f | Unknown: %.4f | Noise: %.4f\n",
                    probabilities[CLASS_ID_TARGET], probabilities[CLASS_ID_UNKNOWN], probabilities[CLASS_ID_NOISE]);
        std::printf("[TEST RESULT] Expected: %d | Predicted: %d | Status: %s\n",
                    vec.expected_class, pred_class, (vec.expected_class == pred_class ? "PASS" : "FAIL"));

        // Stage E: Drive Decision Engine & Actuators
        decisionEngine.processPrediction(pred_class, target_conf);
    }
}

int main() {
    setup();
    loop_test_mode();
    return 0;
}
