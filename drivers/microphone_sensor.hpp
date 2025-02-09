/** MIT License
 * 
 * Copyright (c) 2025 Feb. by Julian Bruin, James Schutte, Richard Kroesen
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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