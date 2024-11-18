#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include <cstdio>

void mainTask(void *params) {
	printf("Boot task started\n");

	for (;;) {
		vTaskDelay(500);
        printf("Heartbeat...\n");
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