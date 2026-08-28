#ifndef PICO2W_MODEL_RUNNER_H
#define PICO2W_MODEL_RUNNER_H

#include <cstdint>
#include "config.h"

// TensorFlow Lite Micro Direct C++ Engine Runner for Pico 2 W (RP2350)
class ModelRunner {
public:
    ModelRunner();
    ~ModelRunner();

    // Initializes TFLite Micro C++ interpreter, registers operators, and allocates tensor arena
    bool begin();

    // Runs INT8 TFLite inference using precomputed or extracted INT8 feature vector (49x40 = 1960 int8_t)
    // Fills probabilities[3] array with float output probabilities [target, unknown, noise]
    bool predict(const int8_t* int8_features, float probabilities[3]);

    // Dequantizes INT8 logit tensor output using model scale and zero point
    void dequantizeOutput(const int8_t* raw_int8_output, float probabilities[3]);

    // Helper methods
    int getPredictedClass(const float probabilities[3]);
    float getTargetConfidence(const float probabilities[3]);
    bool isWakeWordDetected(const float probabilities[3], float threshold = WAKE_WORD_THRESHOLD);

private:
    bool initialized;
};

#endif // PICO2W_MODEL_RUNNER_H
