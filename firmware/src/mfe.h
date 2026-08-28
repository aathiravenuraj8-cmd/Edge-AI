#ifndef MFE_H
#define MFE_H

#include <cstdint>
#include <cstddef>
#include "config.h"

// Log Mel Filterbank Energy Feature Extraction Module
class MFEExtractor {
public:
    MFEExtractor();
    ~MFEExtractor();

    // Computes 49x40 INT8 Mel feature matrix from 1-second (16,000 samples) 16kHz PCM audio
    // Input: pcm_samples (16000 int16_t samples)
    // Output: out_int8_features (1960 int8_t quantized features matching training pipeline)
    bool compute_mfe(const int16_t* pcm_samples, size_t num_samples, int8_t* out_int8_features);
};

#endif // MFE_H
