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