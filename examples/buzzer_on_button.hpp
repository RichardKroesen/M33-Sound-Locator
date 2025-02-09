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
