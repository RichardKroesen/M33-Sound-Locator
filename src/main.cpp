#include <cstdio>
#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"

#include <adc_driver_dma.hpp>
#include <microphone_sensor.hpp>
#include "arm_math.h"
#include "adc_sample_distributor.hpp"
#include "locator.hpp"
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

    uint16_t adcSamples[3];

    sleep_ms(2000);

    const float test_frequencies[] = {500.0f, 1000.0f, 1500.0f, 2000.0f, 3000.0f, 4000.0f, 5000.0f, 6000.0f, 7000.0f, 8000.0f, 9000.0f, 10000.0f};
    Processing::SampleProcessor sample_processor;

    for (;;) {
		gpio_put(PICO_DEFAULT_LED_PIN, 1);
		// vTaskDelay(1 / portTICK_PERIOD_MS);

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

void sampling_print_task(void *param) {
    constexpr uint32_t post_processing_buffer_size = 1000;
    using Microphone0 = SENSOR::Microphone<0, post_processing_buffer_size>;
    auto mic0 = std::make_shared<Microphone0>();
    
    using Microphone1 = SENSOR::Microphone<1, post_processing_buffer_size>;
    auto mic1 = std::make_shared<Microphone1>();

    using Microphone2 = SENSOR::Microphone<2, post_processing_buffer_size>;
    auto mic2 = std::make_shared<Microphone2>();

    constexpr ADC_FREQUENCIES FS = ADC_FREQUENCIES::FS_MAX_PICO;
    constexpr uint32_t total_buffer_size = 60;
    constexpr uint32_t batch_buffer_size = 12;

    static ADC::ADC_Driver_DMA<FS, total_buffer_size, batch_buffer_size> adc_driver;
    static std::unique_ptr<uint16_t[]> buffer = std::make_unique<uint16_t[]>(batch_buffer_size);

    ADC::SampleDistributor<ADC::ISensor, 3> distributor({mic0, mic1, mic2});

    adc_driver.set_notification_task(xTaskGetCurrentTaskHandle());
    adc_driver.start_adc();

    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        adc_driver.copy_buffer(buffer, batch_buffer_size);

        for (uint32_t i = 0; i < batch_buffer_size-2; i = i + 3) {
            printf("%u,%u,%u\n", buffer[i], buffer[i+1], buffer[i+2]);
        }

        distributor.distribute_samples<batch_buffer_size>(buffer);
        adc_driver.start_adc();
    }
}

int main() {
	/* Setup */
	stdio_init_all();

    BaseType_t xReturned, xReturned2;
    TaskHandle_t samp_print_handle;
    TaskHandle_t task;

    xReturned = xTaskCreate(sampling_print_task, "SamplePrint_task", 
        1000, NULL, configMAX_PRIORITIES-1, &samp_print_handle);
    xReturned2 = xTaskCreate(mainTask, "MainThread", 2000, NULL, tskIDLE_PRIORITY, &task);

    if( xReturned != pdPASS ) {
        vTaskDelete(samp_print_handle);
    }
    if (xReturned2 != pdPASS) {
        vTaskDelete(task);
    }

    vTaskStartScheduler();

    for (;;) {}
	return 1;
}