void trigger_buzzer_task(void *param) {
    constexpr uint8_t button_pin = 1; 
    constexpr uint8_t buzzer_pin = 2; 

    static ACTUATOR::Buzzer<buzzer_pin> buz{};
    static SENSOR::ButtonDetector<button_pin> button{}; 

    button.set_notification_task(xTaskGetCurrentTaskHandle());
    button.enable_detection();
    
    static uint32_t counter = 25;
    buz.set_tone_frequency(750);
    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        buz.start_buzzer(counter);

        if (counter >= 750) {
            counter = 50;
        } else {
            counter += 50; 
        }
    }
}
