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

#ifndef SAMPLEPROCESSOR_H
#define SAMPLEPROCESSOR_H

#include "arm_math.h"

namespace Processing {
    // Size of the ADC samples array
    constexpr static inline uint16_t SAMPLE_SIZE = 4096;
    // Sample rate of the ADC in Hz
    constexpr static inline uint32_t SAMPLE_RATE = 166666;
    // Maximum ADC value / 2 because sine is -1 to 1 so 0.0 is 2047.5
    constexpr static inline float AMPLITUDE  = 2047.5f;  
    // Size of each bin in Hz (Sample rate / Sample size)
    constexpr static inline float BIN_SIZE = static_cast<float>(SAMPLE_RATE) / SAMPLE_SIZE;  

    class SampleProcessor {
    private:
        float signal[SAMPLE_SIZE];
        float fft_output[SAMPLE_SIZE];
        static inline arm_rfft_fast_instance_f32 fft_instance;

    public:
        SampleProcessor();

        bool detect_frequency(const float frequency, const uint16_t adc_samples[SAMPLE_SIZE]);
        
    private:
        void process_samples(const uint16_t adc_samples[SAMPLE_SIZE]);
        bool check_frequency(float frequency);
    };

}

#endif // SAMPLEPROCESSOR_H