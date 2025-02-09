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

#ifndef SAMPLE_DISTRIBUTOR_HPP
#define SAMPLE_DISTRIBUTOR_HPP

#include <vector>
#include <memory>
#include <cstdint>
#include <cstdio>
#include "system_references.hpp"

namespace ADC {
    
template <typename MicrophoneType, size_t SensorCount>
class SampleDistributor {
public:
    SampleDistributor(std::vector<std::shared_ptr<MicrophoneType>> mics)
        : microphones(std::move(mics)) {}

    template <const size_t batchSize>
    void distribute_samples(const std::unique_ptr<uint16_t[]>& adcDriverBuffer) {
        static_assert(batchSize % SensorCount == 0, "Batchsize must be divisible by channels.");

        for (size_t i = 0; i < SensorCount; i++) {
            process_mic_samples(adcDriverBuffer, batchSize/SensorCount, microphones[i], SensorCount);
        }
    }

private:
    std::vector<std::shared_ptr<MicrophoneType>> microphones;

    void process_mic_samples(const std::unique_ptr<uint16_t[]>& adcDriverBuffer,
                                  size_t batchSize,
                                  const std::shared_ptr<MicrophoneType>& mic,
                                  size_t channelCount) 
    {
        const uint8_t channel = mic->get_channel();
        std::unique_ptr<uint16_t[]> channelSamples = std::make_unique<uint16_t[]>(batchSize);
        for (size_t i = 0; i < batchSize; i++) {
            channelSamples[i] = adcDriverBuffer[channel + i * channelCount];
        }
        mic->add_samples(channelSamples.get(), batchSize);

    #if DEBUG
        // Print samples for debugging
        printf("Microphone %u Samples:\n", channel);
        for (const auto& sample : channelSamples) {
            printf("%u ", sample);
        }
        printf("\n");
    #endif
    }
};
} // namespace ADC
#endif /* SAMPLE_DISTRIBUTOR_HPP */