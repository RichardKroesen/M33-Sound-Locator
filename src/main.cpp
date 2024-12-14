#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include <cstdio>

#include <adc_driver.hpp>

void mainTask(void *params) {
	printf("Boot task started\n");
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

	static DRIVER::ADC_Driver<static_cast<uint32_t>(ADC_FREQUENCIES::FS_100hz)>test{};
	test.start_adc();

	for (;;) {
		// static int mytemptemptempVar = 430;
		gpio_put(PICO_DEFAULT_LED_PIN, 1);
		vTaskDelay(1 / portTICK_PERIOD_MS);
        printf("%u,%u,%u\n", test.adc_sample_buffer[0], test.adc_sample_buffer[1], test.adc_sample_buffer[2]);
		gpio_put(PICO_DEFAULT_LED_PIN, 0);
		vTaskDelay(1 / portTICK_PERIOD_MS);
	}
}

static inline void vLaunch() {
    TaskHandle_t task;
    xTaskCreate(mainTask, "MainThread", 700, NULL, tskIDLE_PRIORITY, &task);

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