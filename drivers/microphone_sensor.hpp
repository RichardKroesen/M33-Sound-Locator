#ifndef MICROPHONE_SENSOR_HPP
#define MICROPHONE_SENSOR_HPP

#include <cstdint>
#include <array>
#include "stream_buffer.h"

namespace SENSOR {

template <const uint8_t ADC_CHANNEL, const size_t BUFFER_SIZE>

class Microphone {
public:
    Microphone() {
        static_assert(ADC_CHANNEL < 3, "ADC_CHANNEL must be 0, 1, or 2");
        streamBuffer = xStreamBufferCreate(BUFFER_SIZE * sizeof(uint16_t), sizeof(uint16_t));
        configASSERT(streamBuffer);
    }
    
    ~Microphone() = default;

    void add_samples(const uint16_t* samples, size_t sampleCount) {
        size_t bytesToWrite = sampleCount * sizeof(uint16_t);
        size_t bytesWritten = xStreamBufferSend(streamBuffer, samples, bytesToWrite, portMAX_DELAY);
        configASSERT(bytesWritten == bytesToWrite);
    }

    void clear_buffer() {
        xStreamBufferReset(streamBuffer);
    }

    constexpr uint8_t get_channel() const {
        return ADC_CHANNEL;
    }

private:
    StreamBufferHandle_t streamBuffer;

};
}; // namespace SENSOR
#endif /* MICROPHONE_SENSOR_HPP */