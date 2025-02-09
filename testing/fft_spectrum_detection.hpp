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

uint16_t adc_samples[Processing::SAMPLE_SIZE] = {0};

void generate_adc_samples(float frequency) {
    for (int i = 0; i < Processing::SAMPLE_SIZE; i++) {
        const float time = (float)i / Processing::SAMPLE_RATE;

        // Sine wave generation
        const float sine_value = arm_sin_f32(2 * M_PI * frequency * time);

        // Normalize to 0-4095 range
        adc_samples[i] = (uint16_t)((sine_value + 1.0f) * Processing::AMPLITUDE); 
    }

    printf("ADC Samples for %.1fHz: ", frequency);
    for (int i = 0; i < 20; i++) {
        printf("%d ", adc_samples[i]);
    }
    printf("\n");
}

void mainTask(void *params) {
    printf("Boot task started\n");
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    constexpr float test_frequencies[] 
        = {500.0f, 1000.0f, 1500.0f, 2000.0f, 3000.0f, 4000.0f, 5000.0f, 6000.0f, 7000.0f, 8000.0f, 9000.0f, 10000.0f};
    
    static Processing::SampleProcessor sample_processor;

    for (;;) {
        printf("Time: %llu\n", get_absolute_time());
        printf("Sample Size: %d\n", Processing::SAMPLE_SIZE);
        printf("Sample Rate: %d Hz\n\n", Processing::SAMPLE_RATE);

		gpio_put(PICO_DEFAULT_LED_PIN, 1);
        for (float frequency : test_frequencies) {
            generate_adc_samples(frequency);
            sample_processor.detect_frequency(frequency, adc_samples);
        }
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}