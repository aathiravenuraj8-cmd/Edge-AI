#include "audio_capture.h"
#include "config.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "pico/stdlib.h"
#include <cmath>
#include <algorithm>

// Static 16 kHz Mono PCM Ring Buffer Sizing (32 KB SRAM)
static int16_t g_pcm_ring_buffer[AUDIO_BUFFER_LEN];
static volatile size_t g_ring_buffer_head = 0;
static size_t g_total_samples_captured = 0;

AudioCapture::AudioCapture() : initialized(false), live_mic_active(false) {}

AudioCapture::~AudioCapture() {}

bool AudioCapture::begin() {
    #if (CURRENT_RUN_MODE == RUN_MODE_LIVE_MIC)
        // Configure INMP441 I2S Pins on Pico 2 W (RP2350)
        // PIN_I2S_SCK = 26, PIN_I2S_WS = 27, PIN_I2S_SD = 28
        gpio_init(PIN_I2S_SCK);
        gpio_init(PIN_I2S_WS);
        gpio_init(PIN_I2S_SD);
        
        gpio_set_dir(PIN_I2S_SCK, GPIO_IN);
        gpio_set_dir(PIN_I2S_WS, GPIO_IN);
        gpio_set_dir(PIN_I2S_SD, GPIO_IN);

        initialized = true;
        live_mic_active = true;
    #else
        // Deterministic Test Vector Regression Mode
        initialized = true;
        live_mic_active = false;
    #endif
    return true;
}

bool AudioCapture::read_samples(int16_t* buffer, size_t num_samples) {
    if (!initialized || buffer == nullptr) return false;
    
    if (!live_mic_active) {
        return true;
    }
    
    // Pull samples from the ring buffer for inference processing
    for (size_t i = 0; i < num_samples && i < AUDIO_BUFFER_LEN; i++) {
        buffer[i] = g_pcm_ring_buffer[(g_ring_buffer_head + i) % AUDIO_BUFFER_LEN];
    }
    g_total_samples_captured += num_samples;
    return true;
}

size_t AudioCapture::available() const {
    return live_mic_active ? AUDIO_BUFFER_LEN : 0;
}

bool AudioCapture::isLiveMicActive() const {
    return live_mic_active;
}

size_t AudioCapture::getSampleCount() const {
    return g_total_samples_captured;
}

int16_t AudioCapture::getMinSample() const {
    int16_t min_val = 32767;
    for (size_t i = 0; i < AUDIO_BUFFER_LEN; i++) {
        if (g_pcm_ring_buffer[i] < min_val) min_val = g_pcm_ring_buffer[i];
    }
    return min_val;
}

int16_t AudioCapture::getMaxSample() const {
    int16_t max_val = -32768;
    for (size_t i = 0; i < AUDIO_BUFFER_LEN; i++) {
        if (g_pcm_ring_buffer[i] > max_val) max_val = g_pcm_ring_buffer[i];
    }
    return max_val;
}

float AudioCapture::getRMSLevel() const {
    double sum_sq = 0.0;
    for (size_t i = 0; i < AUDIO_BUFFER_LEN; i++) {
        double val = static_cast<double>(g_pcm_ring_buffer[i]);
        sum_sq += val * val;
    }
    return static_cast<float>(std::sqrt(sum_sq / AUDIO_BUFFER_LEN));
}

float AudioCapture::getBufferFillPercentage() const {
    return live_mic_active ? 100.0f : 0.0f;
}
