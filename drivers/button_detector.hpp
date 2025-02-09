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

#ifndef BUTTON_DETECTOR_HPP
#define BUTTON_DETECTOR_HPP

#include <stdint.h>
#include "hardware/gpio.h"

namespace SENSOR {

template <const uint8_t PIN> 
class ButtonDetector {
public:
    ButtonDetector() {
        init();
    }
    
    ~ButtonDetector() = default; 

    static inline void set_notification_task(const TaskHandle_t task) {
        notification_ref = task;
    }

    const static inline void enable_detection(void) {
        if (! init_flag) {
            init();
        }
        gpio_set_irq_enabled_with_callback(PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    }

private:
    static inline bool init_flag = false; 
    static inline TaskHandle_t notification_ref = nullptr;

    static inline void init() {
        if (! init_flag) {
            gpio_init(PIN);
            gpio_set_dir(PIN, false); // Input
            gpio_set_irq_enabled_with_callback(PIN, GPIO_IRQ_EDGE_FALL, false, &gpio_callback);
            init_flag = true;
        }
    }    

    static void gpio_callback(uint gpio, uint32_t events) {
        if(gpio == PIN) {
            BaseType_t xHigherPriorityTaskWoken;
            xHigherPriorityTaskWoken = pdFALSE;
            vTaskNotifyGiveFromISR(notification_ref , &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
};  
} // namespace SENSOR
#endif /* BUTTON_DETECTOR_HPP */