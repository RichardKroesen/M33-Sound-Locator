#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include <cstdio>
#include "tusb.h"

#include <adc_driver_dma.hpp>
#include <adc_driver.hpp>
#include "arm_math.h" // Check of CMSIS DSP inclusion

TaskHandle_t demuxTaskHandle;

void DemuxTask(void *param) {
    ADC::ADC_Driver_DMA<ADC_FREQUENCIES::FS_100k>* adcDriver = static_cast<ADC::ADC_Driver_DMA<ADC_FREQUENCIES::FS_100k>*>(param);
    static uint16_t buffer[900];

    adcDriver->set_notification_task(demuxTaskHandle);
    adcDriver->start_adc();

    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        adcDriver->get_buffer(buffer);
        for (uint32_t i = 0; i < 898; i = i + 3) {
            printf("%u,%u,%u\n", buffer[i], buffer[i+1], buffer[i+2]);
        }
        adcDriver->start_adc();
    }
}

int main() {
	/* Setup */
	stdio_init_all();
	while (! tud_cdc_connected()) {
		printf(".");
		sleep_ms(500);
	}

	static ADC::ADC_Driver_DMA<ADC_FREQUENCIES::FS_100k>* adcDriver = new ADC::ADC_Driver_DMA<ADC_FREQUENCIES::FS_100k>();
    xTaskCreate(DemuxTask, "DemuxTask", 1000, adcDriver, 1, &demuxTaskHandle);

    vTaskStartScheduler();

	while (1);
	return 1;
}