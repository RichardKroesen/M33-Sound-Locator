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
	}
}
