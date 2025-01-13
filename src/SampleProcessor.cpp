#include "SampleProcessor.h"

#include <cstdio>

namespace Processing {

    SampleProcessor::SampleProcessor() : 
        signal{0}, 
        fft_output{0}
    {
        arm_rfft_fast_init_f32(&fft_instance, SAMPLE_SIZE);
    }

    bool SampleProcessor::detect_frequency(const uint16_t adc_samples[SAMPLE_SIZE]) {
        process_samples(adc_samples);
        return check_frequency(2000.0f);
    }

    void SampleProcessor::process_samples(const uint16_t adc_samples[SAMPLE_SIZE]) {
        // Normalize ADC samples to -1.0 to 1.0 range
        for (int i = 0; i < SAMPLE_SIZE; i++) {
            signal[i] = (static_cast<float>(adc_samples[i]) / 4095.0f) * 2.0f - 1.0f;
        }

        // Perform FFT
        arm_rfft_fast_f32(&fft_instance, signal, fft_output, 0);

        // Calculate magnitude
        for (int i = 0; i < SAMPLE_SIZE / 2; i++) {
            float real = fft_output[2 * i];
            float imag = fft_output[2 * i + 1];
            fft_output[i] = sqrtf(real * real + imag * imag);
        }
    }

    bool SampleProcessor::check_frequency(float frequency) {
        int largest_bin = 0;
        for (int i = 1; i < SAMPLE_SIZE / 2; i++) {
            if (fft_output[i] > fft_output[largest_bin]) {
                largest_bin = i;
            }
        }

        const float bin_starting_freq = largest_bin * BIN_SIZE;
        const float bin_ending_freq = bin_starting_freq + BIN_SIZE;

        if (bin_starting_freq <= frequency && frequency < bin_ending_freq) {
            printf("Found frequency: %.1fHz, Magnitude: %.2f\n\n", bin_starting_freq, fft_output[largest_bin]);
            return true;
        } else {
            // print the frequency with the highest magnitude
            printf("Did not find specified frequency %.1f\nHighest frequency: %.1fHz, Magnitude: %.2f\n\n", frequency, bin_starting_freq, fft_output[largest_bin]);
            return false;
        }
    }
}