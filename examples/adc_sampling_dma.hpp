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

#include "adc_sample_distributor.hpp"

template <typename MicrophoneType>
void processMicrophoneSamples(
    const std::unique_ptr<uint16_t[]>& adcDriverBuffer,
    size_t totalBatchSize,
    const std::shared_ptr<MicrophoneType>& mic,
    size_t channelCount) 
{
    const uint8_t channel = mic->get_channel();
    const size_t batchSizePerChannel = totalBatchSize / channelCount;
    std::vector<uint16_t> channelSamples(batchSizePerChannel);
    for (size_t i = 0; i < batchSizePerChannel; i++) {
        channelSamples[i] = adcDriverBuffer[channel + i * channelCount];
    }
    mic->add_samples(channelSamples.data(), batchSizePerChannel);

    // Print samples for debugging
    printf("M%u: ", channel);
    for (size_t i = 0; i < batchSizePerChannel; i++) {
        printf("%u ", channelSamples[i]);
    }
    printf("\n");
}

template <size_t SensorCount>
void distributeSamples(
    const std::unique_ptr<uint16_t[]>& adcDriverBuffer,
    size_t batchSize,
    const std::shared_ptr<Microphone0>& mic0,
    const std::shared_ptr<Microphone1>& mic1,
    const std::shared_ptr<Microphone2>& mic2) 
{
    static constexpr size_t channelCount = SensorCount;
    processMicrophoneSamples(adcDriverBuffer, batchSize, mic0, channelCount);
    processMicrophoneSamples(adcDriverBuffer, batchSize, mic1, channelCount);
    processMicrophoneSamples(adcDriverBuffer, batchSize, mic2, channelCount);
}

void samp_print(void *param) {
    constexpr ADC_FREQUENCIES FS = (ADC_FREQUENCIES::FS_MAX_PICO);
    constexpr uint32_t total_buffer_size = 90;
    constexpr uint32_t batch_buffer_size = 9; // Size which notifies the task when complete. 

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
        distributeSamples<3>(buffer, batch_buffer_size, mic0, mic1, mic2);

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
        1000, NULL, configMAX_PRIORITIES-1, &samp_print_handle);
    if( xReturned != pdPASS ) {
        vTaskDelete(samp_print_handle);
    }

    vTaskStartScheduler();

	while (1);
	return 1;
}