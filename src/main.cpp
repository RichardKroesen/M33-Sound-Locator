#include <vector>
#include <memory>
#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include <cstdio>
#include "tusb.h"

#include <adc_driver_dma.hpp>
#include <microphone_sensor.hpp>
#include "arm_math.h" // Check of CMSIS DSP inclusion

using Microphone0 = SENSOR::Microphone<0, 200>;
using Microphone1 = SENSOR::Microphone<1, 200>;
using Microphone2 = SENSOR::Microphone<2, 200>;

template <size_t SensorCount>
void distributeSamples(
    const std::unique_ptr<uint16_t[]>& adcDriverBuffer,
    size_t batchSize,
    std::shared_ptr<Microphone0> mic0,
    std::shared_ptr<Microphone1> mic1,
    std::shared_ptr<Microphone2> mic2) 
{
    // Process mic0
    {
        const uint8_t channel = mic0->get_channel();
        std::vector<uint16_t> channelSamples(batchSize);
        for (size_t i = 0; i < batchSize; ++i) {
            channelSamples[i] = adcDriverBuffer[channel + i * 3];
        }
        mic0->add_samples(channelSamples.data(), batchSize);

        // Print samples for mic0
        printf("Microphone 0 Samples:\n");
        for (size_t i = 0; i < batchSize; ++i) {
            printf("%u ", channelSamples[i]);
        }
        printf("\n");
    }

    // Process mic1
    {
        const uint8_t channel = mic1->get_channel();
        std::vector<uint16_t> channelSamples(batchSize);
        for (size_t i = 0; i < batchSize; ++i) {
            channelSamples[i] = adcDriverBuffer[channel + i * 3];
        }
        mic1->add_samples(channelSamples.data(), batchSize);

        printf("Microphone 1 Samples:\n");
        for (size_t i = 0; i < batchSize; ++i) {
            printf("%u ", channelSamples[i]);
        }
        printf("\n");
    }

    // Process mic2
    {
        const uint8_t channel = mic2->get_channel();
        std::vector<uint16_t> channelSamples(batchSize);
        for (size_t i = 0; i < batchSize; ++i) {
            channelSamples[i] = adcDriverBuffer[channel + i * 3];
        }
        mic2->add_samples(channelSamples.data(), batchSize);

        printf("Microphone 2 Samples:\n");
        for (size_t i = 0; i < batchSize; ++i) {
            printf("%u ", channelSamples[i]);
        }
        printf("\n");
    }
}

void samp_print(void *param) {
    constexpr ADC_FREQUENCIES FS = (ADC_FREQUENCIES::FS_MAX_PICO);
    constexpr uint32_t total_buffer_size = 600;
    constexpr uint32_t batch_buffer_size = 12; // Size which notifies the task when complete. 

	static ADC::ADC_Driver_DMA<FS, total_buffer_size, batch_buffer_size> adc_driver 
        = ADC::ADC_Driver_DMA<FS, total_buffer_size, batch_buffer_size>();

    static std::unique_ptr<uint16_t[]> buffer = std::make_unique<uint16_t[]>(batch_buffer_size);
    auto mic0 = std::make_shared<Microphone0>();
    auto mic1 = std::make_shared<Microphone1>();
    auto mic2 = std::make_shared<Microphone2>();

    adc_driver.set_notification_task(xTaskGetCurrentTaskHandle());
    adc_driver.start_adc();

    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        adc_driver.copy_buffer(buffer, batch_buffer_size);
        for (uint32_t i = 0; i < batch_buffer_size-2; i = i + 3) {
            printf("%u,%u,%u\n", buffer[i], buffer[i+1], buffer[i+2]);
        }
        distributeSamples<3>(buffer, batch_buffer_size/3, mic0, mic1, mic2);

        adc_driver.start_adc();
    }
}

int main() {
	/* Setup */
	stdio_init_all();
	while (! tud_cdc_connected()) {
		printf(".");
		sleep_ms(500);
	}

    BaseType_t xReturned;
    TaskHandle_t samp_print_handle;

    xReturned = xTaskCreate(samp_print, "SamplePrint_task", 
        1000, NULL, 1, &samp_print_handle);
    if( xReturned != pdPASS ) {
        vTaskDelete(samp_print_handle);
    }

    vTaskStartScheduler();

	while (1);
	return 1;
}