#include "model_runner.h"
#include "model_data.h"
#include <Arduino.h>

// Include EloquentTinyML for ESP32 Arduino / Wokwi TFLite Micro execution
#include <EloquentTinyML.h>

// Instantiate EloquentTinyML TFLite Micro interpreter
// Input Size: 1960 (49x40), Output Size: 3, Tensor Arena: 20KB
static Eloquent::TF::Sequential<MODEL_INPUT_SIZE, MODEL_NUM_CLASSES, TENSOR_ARENA_SIZE> tf;

ModelRunner::ModelRunner() : initialized(false) {}

ModelRunner::~ModelRunner() {}

bool ModelRunner::begin() {
    Serial.println("[TFLite Micro] Initializing HeroArise INT8 Model...");
    Serial.printf("[TFLite Micro] Model Size: %u bytes\n", hero_arise_model_len);
    Serial.printf("[TFLite Micro] Tensor Arena Allocation: %d bytes\n", TENSOR_ARENA_SIZE);

    tf.begin(hero_arise_model);

    if (!tf.initialized()) {
        Serial.println("[TFLite Micro] ERROR: Failed to initialize TFLite Micro interpreter!");
        initialized = false;
        return false;
    }

    Serial.println("[TFLite Micro] Model successfully initialized!");
    initialized = true;
    return true;
}

bool ModelRunner::predict(const int8_t* int8_features, float probabilities[3]) {
    if (!initialized && !begin()) {
        Serial.println("[TFLite Micro] ERROR: Predict called on uninitialized model!");
        return false;
    }

    // Run INT8 TFLite Micro inference
    // EloquentTinyML automatically passes INT8 features to tensor input and dequantizes output
    tf.predict((uint8_t*)int8_features, probabilities);

    // Apply Softmax normalization if needed to ensure probabilities sum to 1.0
    float sum = probabilities[0] + probabilities[1] + probabilities[2];
    if (sum > 0.0f) {
        probabilities[0] /= sum;
        probabilities[1] /= sum;
        probabilities[2] /= sum;
    }

    return true;
}

int ModelRunner::getPredictedClass(const float probabilities[3]) {
    int max_idx = 0;
    float max_val = probabilities[0];
    for (int i = 1; i < MODEL_NUM_CLASSES; i++) {
        if (probabilities[i] > max_val) {
            max_val = probabilities[i];
            max_idx = i;
        }
    }
    return max_idx;
}

float ModelRunner::getTargetConfidence(const float probabilities[3]) {
    return probabilities[CLASS_ID_TARGET];
}

bool ModelRunner::isWakeWordDetected(const float probabilities[3], float threshold) {
    int top_cls = getPredictedClass(probabilities);
    float target_conf = getTargetConfidence(probabilities);
    return (top_cls == CLASS_ID_TARGET) && (target_conf >= threshold);
}
