
#include "locator.hpp"

#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include <cstdio>

#include <adc_driver.hpp>
#include "arm_math.h" // Check of CMSIS DSP inclusion

void mainTask(void *params) {
    printf("Boot task started\n");
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

	static ADC::ADC_Driver<static_cast<uint32_t>(ADC_FREQUENCIES::FS_1k)>test{};
	test.start_adc();
    uint16_t adcSamples[3];

	for (;;) {
		static int mytemptemptempVar = 430;
		gpio_put(PICO_DEFAULT_LED_PIN, 1);
		// vTaskDelay(1 / portTICK_PERIOD_MS);

        if (test.readSamples(adcSamples, 3)) {
            printf("%u,%u,%u\n", adcSamples[0], adcSamples[1], adcSamples[2]);
        } else {
            printf("Failed to read ADC samples.\n");
        }

		gpio_put(PICO_DEFAULT_LED_PIN, 0);
		// vTaskDelay(1 / portTICK_PERIOD_MS);

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
    xTaskCreate(mainTask, "MainThread", 1000, NULL, 2, &task);

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
