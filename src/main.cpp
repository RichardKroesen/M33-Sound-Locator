
#include <cstdio>

#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"

#include "adc_driver.hpp"
#include "locator.hpp"

#include "SampleProcessor.h"
#include "arm_math.h" // Check of CMSIS DSP inclusion

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

	static ADC::ADC_Driver<static_cast<uint32_t>(ADC_FREQUENCIES::FS_10k)>test{};
	test.start_adc();
    uint16_t adcSamples[3];

    sleep_ms(2000);

    const float test_frequencies[] = {500.0f, 1000.0f, 1500.0f, 2000.0f, 3000.0f, 4000.0f, 5000.0f, 6000.0f, 7000.0f, 8000.0f, 9000.0f, 10000.0f};
    Processing::SampleProcessor sample_processor;

    for (;;) {
		gpio_put(PICO_DEFAULT_LED_PIN, 1);
		// vTaskDelay(1 / portTICK_PERIOD_MS);

        if (test.readSamples(adcSamples, 3)) {
            printf("%u,%u,%u\n", adcSamples[0], adcSamples[1], adcSamples[2]);
        } else {
            printf("Failed to read ADC samples.\n");
        }

		gpio_put(PICO_DEFAULT_LED_PIN, 0);

        printf("Time: %llu\n", get_absolute_time());
        printf("Sample Size: %d\n", Processing::SAMPLE_SIZE);
        printf("Sample Rate: %d Hz\n\n", Processing::SAMPLE_RATE);

        for (float frequency : test_frequencies) {
            generate_adc_samples(frequency);
            sample_processor.detect_frequency(frequency, adc_samples);
        }

        ALGORITHM::SensorConfig sensors(
            ALGORITHM::vec2_t { 450, 90 },
            ALGORITHM::vec2_t { 420, 380 },
            ALGORITHM::vec2_t { 50, 50 }
        );

        sensors.measured(0, 420);
        sensors.measured(1, 420 + 0.5078);
        sensors.measured(2, 420 + 0.6206);

        ALGORITHM::vec2_t output = { 0, 0 };
        bool success = sensors.calculate(&output);

        printf("Calculated position, (%f, %f) did succeed %d\n", output.x, output.y, success);
        printf("Expected (350, 120)\n");
    }
}

static inline void vLaunch() {
    TaskHandle_t task;
    xTaskCreate(mainTask, "MainThread", 2000, NULL, tskIDLE_PRIORITY, &task);

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

int main() {
    /* Setup */
    stdio_init_all();
    vLaunch();

	while (1);
	return 1;
}
