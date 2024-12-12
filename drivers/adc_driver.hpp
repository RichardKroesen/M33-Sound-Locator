#ifndef ADC_DRIVER_HPP
#define ADC_DRIVER_HPP

#include "hardware/gpio.h"
#include "hardware/adc.h"

namespace DRIVER {
    
class ADC_Driver {
public: 
    ADC_Driver() {
        init();
    }

    ~ADC_Driver() = default; 

static inline volatile uint16_t adc_value = 0;

protected: 
    typedef struct {
        const uint8_t pin;
        const uint8_t adc_reference;
    } pin_reference_t;

    constexpr static pin_reference_t pinReferences[3] = {
        {26, 0},
        {27, 1},
        {28, 2}
    };

private: 
    static inline bool init_flag = false;

    static inline void init() {
        if (! init_flag) {
            adc_init();
            
            // for (auto &p : pinReferences) {
            //     adc_gpio_init(p.pin);
            // }
            
            gpio_set_dir(pinReferences[0].pin, false);
            gpio_disable_pulls(pinReferences[0].pin);
            adc_gpio_init(pinReferences[0].pin);
            adc_select_input(pinReferences[0].adc_reference); // Initial Channel. 

            adc_irq_set_enabled(true);
            adc_fifo_setup(
                true,
                false,
                1,
                false,
                false
            );								
            
            irq_set_exclusive_handler(ADC_IRQ_FIFO, adc_handler);
            irq_set_priority (ADC_IRQ_FIFO, PICO_HIGHEST_IRQ_PRIORITY);		
            irq_set_enabled(ADC_IRQ_FIFO, true);

            adc_set_clkdiv(3000);		
            adc_run(true);
            init_flag = true;
        }
    }

    static void adc_handler() {
        if (! adc_fifo_is_empty()) {
            adc_value = adc_fifo_get();
        }
    }
};
};
#endif // ADC_DRIVER_HPP