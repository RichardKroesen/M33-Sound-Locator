#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include <cstdio>
#include "tusb.h"

#include <adc_driver_dma.hpp>
#include <adc_driver_irq.hpp>
#include "arm_math.h" // Check of CMSIS DSP inclusion

TaskHandle_t demuxTaskHandle;

// void DemuxTask(void *param) {
//     ADC::ADC_Driver<ADC_FREQUENCIES::FS_1k>* adcDriver = static_cast<ADC::ADC_Driver<ADC_FREQUENCIES::FS_1k>*>(param);
//     static uint16_t buffer[500];

//     adcDriver->set_notification_task(demuxTaskHandle);
//     adcDriver->start_adc();

//     while (1) {
//         ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
//         adcDriver->get_buffer(buffer);
//         for (uint32_t i = 0; i < 500-2; i = i + 3) {
//             printf("%u,%u,%u\n", buffer[i], buffer[i+1], buffer[i+2]);
//         }
//         adcDriver->start_adc();
//     }
// }

void mainTask(void *params) {
    ADC::ADC_Driver<ADC_FREQUENCIES::FS_1k>* adcDriver = static_cast<ADC::ADC_Driver<ADC_FREQUENCIES::FS_1k>*>(params);

    adcDriver->start_adc();

    uint16_t adcSamples[201];

	for (;;) {
		gpio_put(PICO_DEFAULT_LED_PIN, 1);
		// vTaskDelay(1 / portTICK_PERIOD_MS);

        if (adcDriver->readSamples(adcSamples, 201)) {
            adcDriver->stop_adc();
            for (uint16_t i = 0; i < 199; i = i + 3) {
                printf("%u,%u,%u\n", adcSamples[i], adcSamples[i+1], adcSamples[i + 2]);
            }
            adcDriver->start_adc();
        }

		gpio_put(PICO_DEFAULT_LED_PIN, 0);
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

int main() {
	/* Setup */
	stdio_init_all();
	while (! tud_cdc_connected()) {
		printf(".");
		sleep_ms(500);
	}

	// static ADC::ADC_Driver_DMA<ADC_FREQUENCIES::FS_100k>* adcDriver = new ADC::ADC_Driver_DMA<ADC_FREQUENCIES::FS_100k>();
    static ADC::ADC_Driver<ADC_FREQUENCIES::FS_1k>* adcDriver = new ADC::ADC_Driver<ADC_FREQUENCIES::FS_1k>();

    xTaskCreate(mainTask, "DemuxTask", 1200, adcDriver, 1, &demuxTaskHandle);

    vTaskStartScheduler();

	while (1);
	return 1;
}