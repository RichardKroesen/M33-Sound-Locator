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