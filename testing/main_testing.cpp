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