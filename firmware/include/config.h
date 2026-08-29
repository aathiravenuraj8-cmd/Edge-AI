#ifndef PICO2W_CONFIG_H
#define PICO2W_CONFIG_H

// Operational Mode Selection Configuration
#define RUN_MODE_TEST_VECTOR 0  // Deterministic precomputed INT8 test vector regression mode
#define RUN_MODE_LIVE_MIC    1  // Live INMP441 I2S digital microphone audio capture mode
#define CURRENT_RUN_MODE    RUN_MODE_LIVE_MIC  // Deterministic precomputed INT8 test vector regression mode

// Microcontroller Hardware Pins for Raspberry Pi Pico 2 W (RP2350)
#define PIN_OLED_SDA       4   // RP2350 I2C0 SDA (GP4, Pin 6)
#define PIN_OLED_SCL       5   // RP2350 I2C0 SCL (GP5, Pin 7)
#define PIN_LED_GREEN      14  // GPIO14 for Wake Word Alert LED (Pin 19)
#define PIN_LED_RED        15  // GPIO15 for Listening Idle LED (Pin 20)
#define PIN_BUZZER         16  // GPIO16 for Piezo Buzzer Alert (Pin 21)

// Target I2S Digital Microphone Pins (INMP441) for Pico 2 W
#define PIN_I2S_SCK        26  // I2S Bit Clock (GP26, Pin 31)
#define PIN_I2S_WS         27  // I2S Word Select / LR Clock (GP27, Pin 32)
#define PIN_I2S_SD         28  // I2S Serial Data Out (GP28, Pin 34)

// Display Configuration
#define OLED_I2C_ADDRESS   0x3C
#define OLED_SCREEN_WIDTH  128
#define OLED_SCREEN_HEIGHT 64

// Audio Pipeline Parameters (16kHz, 16-bit Mono PCM)
#define SAMPLE_RATE        16000
#define AUDIO_CHANNELS     1
#define BIT_DEPTH          16
#define AUDIO_BUFFER_SEC   1.0f
#define AUDIO_BUFFER_LEN   16000

// Mel Filterbank Feature Parameters (Training Mirror)
#define MFE_N_FFT          512
#define MFE_WIN_LENGTH     480  // 30 ms @ 16 kHz
#define MFE_HOP_LENGTH     320  // 20 ms @ 16 kHz
#define MFE_N_MELS         40   // 40 Mel bands
#define MFE_F_MIN          20.0f
#define MFE_F_MAX          8000.0f
#define MFE_LOG_OFFSET     1e-6f

// Model Input & Output Tensor Parameters
#define MODEL_INPUT_FRAMES 49
#define MODEL_INPUT_MELS   40
#define MODEL_INPUT_SIZE   (MODEL_INPUT_FRAMES * MODEL_INPUT_MELS) // 1960 int8_t
#define MODEL_NUM_CLASSES  3

// Class ID Definitions
#define CLASS_ID_TARGET    0  // "Hero Arise"
#define CLASS_ID_UNKNOWN   1  // "unknown_words"
#define CLASS_ID_NOISE     2  // "background_noise"

// Wake-Word Detection Threshold
#define WAKE_WORD_THRESHOLD 0.75f

// INT8 Model Quantization Parameters (Verified from tflite_model_spec.json)
#define INPUT_SCALE        0.06907098f
#define INPUT_ZERO_POINT   72
#define OUTPUT_SCALE       0.00390625f
#define OUTPUT_ZERO_POINT  -128

// Tensor Arena Allocation Size for TensorFlow Lite Micro Engine (Pico 2 W SRAM)
#define TENSOR_ARENA_SIZE  (20 * 1024)

#endif // PICO2W_CONFIG_H
