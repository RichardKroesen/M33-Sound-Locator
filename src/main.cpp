#include "main.hpp"

template <const size_t NUM_TASKS>
static inline void vLaunch() {
	BaseType_t xReturned[NUM_TASKS];
	TaskHandle_t taskHandle[NUM_TASKS];

    xReturned[0] = xTaskCreate(heart_beat_task, 
		"Heartbeat", 
		configMINIMAL_STACK_SIZE, 
		NULL, tskIDLE_PRIORITY, &taskHandle[0]);

	xReturned[1] = xTaskCreate(trigger_buz_on_button_task,
		"BuzzerTrigger", 
		configMINIMAL_STACK_SIZE, 
		NULL, configMAX_PRIORITIES-2, &taskHandle[1]);

	// Check the tasks are created, otherwise delete them:
	for (size_t i = 0; i < NUM_TASKS; i++) {
		if (xReturned[i] != pdPASS) {
			vTaskDelete(taskHandle[i]);
		}
	}

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

int main() {
	/* Setup */
	stdio_init_all();
	vLaunch<1>();

    for (;;) {}
	return 1;
}