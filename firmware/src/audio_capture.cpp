#include "audio_capture.h"

AudioCapture::AudioCapture() : initialized(false), live_mic_active(false) {}

AudioCapture::~AudioCapture() {}

bool AudioCapture::begin() {
    // Note: Live INMP441 I2S microphone capture initialization is stubbed for Task 11.
    // Deterministic test-vector verification mode is active for Milestone 10.
    initialized = true;
    live_mic_active = false;
    return true;
}

bool AudioCapture::read_samples(int16_t* buffer, size_t num_samples) {
    if (!initialized) return false;
    
    if (!live_mic_active) {
        // In test mode, live microphone reading is bypassed
        return true;
    }
    
    // RP2350 I2S read implementation will populate buffer with 16kHz mono PCM
    return true;
}

size_t AudioCapture::available() const {
    return live_mic_active ? AUDIO_BUFFER_LEN : 0;
}

bool AudioCapture::isLiveMicActive() const {
    return live_mic_active;
}
