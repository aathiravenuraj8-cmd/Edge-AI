#ifndef MODEL_RUNNER_H
#define MODEL_RUNNER_H

#include <cstdint>
#include "config.h"

class ModelRunner {
public:
    ModelRunner();
    ~ModelRunner();

    // Initializes TFLite Micro interpreter, registers operators, and allocates tensor arena
    bool begin();

    // Runs INT8 TFLite inference using precomputed INT8 feature vector (49x40 = 1960 int8_t)
    // Fills probabilities[3] array with float output probabilities [target, unknown, noise]
    bool predict(const int8_t* int8_features, float probabilities[3]);

    // Helper methods
    int getPredictedClass(const float probabilities[3]);
    float getTargetConfidence(const float probabilities[3]);
    bool isWakeWordDetected(const float probabilities[3], float threshold = WAKE_WORD_THRESHOLD);

private:
    bool initialized;
};

#endif // MODEL_RUNNER_H
