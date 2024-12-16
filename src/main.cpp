#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include <cstdio>

#include "arm_math.h" // Check of CMSIS DSP inclusion

void mainTask(void *params) {
	printf("Boot task started\n");
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

	for (;;) {
		static int mytemptemptempVar = 430;
		gpio_put(PICO_DEFAULT_LED_PIN, 1);
		vTaskDelay(500);
        printf("Heartbeat...\n");
		gpio_put(PICO_DEFAULT_LED_PIN, 0);
		vTaskDelay(500);
	}
}

static inline void vLaunch() {
    TaskHandle_t task;
    xTaskCreate(mainTask, "MainThread", 500, NULL, tskIDLE_PRIORITY, &task);

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

int main() {
	/* Setup */
	stdio_init_all();
    vLaunch();

	while(1);
	return 1;
}