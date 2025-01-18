#include <cstdio>
#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"

#include "arm_math.h"

#include "fft_spectrum_detection.hpp"
#include "sampling_logging.hpp"
#include "locator_determination.hpp"

int main() {
	/* Setup */
	stdio_init_all();

    BaseType_t xReturned, xReturned2;
    TaskHandle_t samp_print_handle;
    TaskHandle_t task;

    xReturned = xTaskCreate(sampling_print_task, "SamplePrint_task", 
        1000, NULL, configMAX_PRIORITIES-1, &samp_print_handle);
    xReturned2 = xTaskCreate(mainTask, "MainThread", 2000, NULL, tskIDLE_PRIORITY, &task);
    xTaskCreate(locator_determination, "locator", 500, NULL, tskIDLE_PRIORITY, nullptr);

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