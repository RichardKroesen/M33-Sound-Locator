#ifndef MICROPHONE_SENSOR_HPP
#define MICROPHONE_SENSOR_HPP

#include <cstdint>
#include <array>
#include "stream_buffer.h"
#include "adc_sensor_interface.hpp"

namespace SENSOR {

template <const uint8_t ADC_CHANNEL, const size_t BUFFER_SIZE>

class Microphone : public ADC::ISensor {
public:
    Microphone() {
        static_assert(ADC_CHANNEL < 3, "ADC_CHANNEL must be 0, 1, or 2");
        streamBuffer = xStreamBufferCreate(BUFFER_SIZE * sizeof(uint16_t), sizeof(uint16_t));
        configASSERT(streamBuffer);
    }
    
    ~Microphone() = default;

    void add_samples(const uint16_t* samples, size_t sampleCount) override {
        size_t bytesToWrite = sampleCount * sizeof(uint16_t);
        size_t availableSpace = xStreamBufferSpacesAvailable(streamBuffer);
        
        if (availableSpace < bytesToWrite) {
            size_t discardSize = bytesToWrite - availableSpace;
            uint8_t discardBuffer[64]; // Fixed-size buffer for discards
            size_t totalDiscarded = 0;

            while (totalDiscarded < discardSize) {
                size_t chunkSize = std::min(sizeof(discardBuffer), discardSize - totalDiscarded);
                size_t bytesRead = xStreamBufferReceive(streamBuffer, discardBuffer, chunkSize, 0);
                totalDiscarded += bytesRead;

                configASSERT(bytesRead > 0); // Prevent inf loop
            }
        }
        size_t bytesWritten = xStreamBufferSend(streamBuffer, samples, bytesToWrite, 0);
        configASSERT(bytesWritten == bytesToWrite);
    }

    void clear_buffer() {
        xStreamBufferReset(streamBuffer);
    }

    constexpr uint8_t get_channel() const override {
        return ADC_CHANNEL;
    }

private:
    StreamBufferHandle_t streamBuffer;
};
}; // namespace SENSOR
#endif /* MICROPHONE_SENSOR_HPP */