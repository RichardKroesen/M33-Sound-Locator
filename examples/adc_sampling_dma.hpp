#include <memory>
#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include <cstdio>
#include "tusb.h"

#include <adc_driver_dma.hpp>
#include <microphone_sensor.hpp>
#include "arm_math.h" // Check of CMSIS DSP inclusion

void samp_print(void *param) {
    constexpr ADC_FREQUENCIES FS = (ADC_FREQUENCIES::FS_MAX_PICO);
    constexpr uint32_t total_buffer_size = 600;
    constexpr uint32_t batch_buffer_size = 3; // Size which notifies the task when complete. 

	static ADC::ADC_Driver_DMA<FS, total_buffer_size, batch_buffer_size> adc_driver 
        = ADC::ADC_Driver_DMA<FS, total_buffer_size, batch_buffer_size>();

    static std::unique_ptr<uint16_t[]> buffer = std::make_unique<uint16_t[]>(batch_buffer_size);

    adc_driver.set_notification_task(xTaskGetCurrentTaskHandle());
    adc_driver.start_adc();

    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        adc_driver.copy_buffer(buffer, batch_buffer_size);
        for (uint32_t i = 0; i < batch_buffer_size-2; i = i + 3) {
            printf("%u,%u,%u\n", buffer[i], buffer[i+1], buffer[i+2]);
        }
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