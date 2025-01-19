#include "locator.hpp"

void locator_determination(void *params) {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    static ALGORITHM::SensorConfig sensors(
        ALGORITHM::vec2_t { 450, 90 },
        ALGORITHM::vec2_t { 420, 380 },
        ALGORITHM::vec2_t { 50, 50 }
    );

    for (;;) {
		gpio_put(PICO_DEFAULT_LED_PIN, 1);

        sensors.measured(0, 420);
        sensors.measured(1, 420 + 0.5078);
        sensors.measured(2, 420 + 0.6206);

        ALGORITHM::vec2_t output = { 0, 0 };
        bool success = sensors.calculate(&output);
		gpio_put(PICO_DEFAULT_LED_PIN, 0);

        printf("Calculated position, (%f, %f) did succeed %d\n", output.x, output.y, success);
        printf("Expected (350, 120)\n");

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}