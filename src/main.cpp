/** MIT License
 * 
 * Copyright (c) 2025 Feb. by Julian Bruin, James Schutte, Richard Kroesen
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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