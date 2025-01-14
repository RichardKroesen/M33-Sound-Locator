
#include "locator.hpp"

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

        ALGORITHM::SensorConfig sensors(
            ALGORITHM::vec2_t { 450, 90 },
            ALGORITHM::vec2_t { 420, 380 },
            ALGORITHM::vec2_t { 50, 50 }
        );

        sensors.measured(0, 420);
        sensors.measured(1, 420 + 0.5078);
        sensors.measured(2, 420 + 0.6206);

        ALGORITHM::vec2_t output = { 0, 0 };
        bool success = sensors.calculate(&output);

        printf("Calculated position, (%f, %f) did succeed %d\n", output.x, output.y, success);
        printf("Expected (350, 120)\n");
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
