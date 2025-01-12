
#include "algorithm.hpp"

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

        // Basic and shitty test for the algorithm
        Algorithm::sensor_t sensors[3];
        sensors[0].position = { 450, 90 };
        sensors[1].position = { 420, 380 };
        sensors[2].position = { 50, 50 };

        sensors[0].duration = 420; // I'm super funny
        sensors[1].duration = sensors[0].duration + 0.5078;
        sensors[2].duration = sensors[0].duration + 0.6206;

        Algorithm::vec2_t output = { 0, 0 };
        bool succeeded = Algorithm::calc_position(sensors, &output);

        printf("Calculated position, (%f, %f) did succeed %d\n", output.x, output.y, succeeded);
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
