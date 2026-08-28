#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H

#include <cstdint>
#include <cstddef>
#include "config.h"

// Hardware Abstraction Layer for Audio Capture (INMP441 I2S Microphone / Test Vectors)
class AudioCapture {
public:
    AudioCapture();
    ~AudioCapture();

    // Initializes audio capture peripherals (I2S / DMA / PIO on RP2350)
    bool begin();

    // Reads raw 16 kHz mono 16-bit PCM audio samples into target buffer
    bool read_samples(int16_t* buffer, size_t num_samples);

    // Returns available unread PCM sample count in ring buffer
    size_t available() const;

    // Returns whether live microphone capture is active vs test vector mode
    bool isLiveMicActive() const;

    // Diagnostic Telemetry Methods for Live Audio Validation
    size_t getSampleCount() const;
    int16_t getMinSample() const;
    int16_t getMaxSample() const;
    float getRMSLevel() const;
    float getBufferFillPercentage() const;

private:
    bool initialized;
    bool live_mic_active;
};

#endif // AUDIO_CAPTURE_H
