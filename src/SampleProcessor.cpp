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

#include "SampleProcessor.h"

#include <cstdio>

namespace Processing {

    SampleProcessor::SampleProcessor() : 
        signal{0}, 
        fft_output{0}
    {
        arm_rfft_fast_init_f32(&fft_instance, SAMPLE_SIZE);
    }

    bool SampleProcessor::detect_frequency(const float frequency, const uint16_t adc_samples[SAMPLE_SIZE]) {
        process_samples(adc_samples);
        return check_frequency(frequency);
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

    bool SampleProcessor::check_frequency(const float frequency) {
        unsigned largest_bin = 0;
        for (int i = 1; i < SAMPLE_SIZE / 2; i++) {
            if (fft_output[i] > fft_output[largest_bin]) {
                largest_bin = i;
            }
        }

        if (largest_bin != 0) {
            largest_bin--;
        }

        const float bin_starting_freq = largest_bin * BIN_SIZE;
        const float bin_ending_freq = bin_starting_freq + BIN_SIZE * 2.0f;

        if (bin_starting_freq <= frequency && frequency < bin_ending_freq) {
            // printf("Found frequency between %.1fHz and %.1f, Magnitude: %.2f\n\n", bin_starting_freq, bin_ending_freq, fft_output[largest_bin]);
            return true;
        } else {
            // printf("Did not find specified frequency between %.1fHz and %.1f, Magnitude: %.2f\n\n", bin_starting_freq, bin_ending_freq, fft_output[largest_bin]);
            return false;
        }
    }
}