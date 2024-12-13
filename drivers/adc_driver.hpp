#ifndef ADC_DRIVER_HPP
#define ADC_DRIVER_HPP

#include <math.h>
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "system_references.hpp"

namespace DRIVER {

template<const uint32_t SAMPLE_RATE>
class ADC_Driver {
public: 
    ADC_Driver() {
        init();
    }

    ~ADC_Driver() = default; 

    const static inline bool start_adc() {
        if (init_flag) {
            adc_select_input(pinReferences[0].adc_reference); // Initial Channel. 
            adc_irq_set_enabled(true);
            irq_set_enabled(ADC_IRQ_FIFO, true);
            adc_sample_buffer[0] = 0;
            adc_sample_buffer[1] = 0;
            adc_sample_buffer[2] = 0;
            adc_fifo_drain();

            adc_run(true);
            return true;
        }
        return false; 
    }
    // static inline void stop_adc();
    // const static inline void read_samples();

    static inline volatile uint16_t adc_sample_buffer[3] = {0, 0, 0};

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
    // 48 MHz/96 Cycles / DIV = Sampling Frequency.
    constexpr static inline uint32_t clk_divider = std::round((48'000'000/96) / SAMPLE_RATE);

    static inline void init() {
        if (! init_flag) {
            adc_init();
            
            for (auto &p : pinReferences) {
                gpio_set_dir(p.pin, false);
                gpio_disable_pulls(p.pin);
                adc_gpio_init(p.pin);
            }

            adc_set_round_robin((1 << pinReferences[0].adc_reference | 
                 1 << pinReferences[1].adc_reference | 
                 1 << pinReferences[2].adc_reference)
            );

            adc_fifo_setup(
                true,
                false,
                3,
                false,
                false
            );								
            
            irq_set_exclusive_handler(ADC_IRQ_FIFO, adc_handler);
            irq_set_priority (ADC_IRQ_FIFO, PICO_HIGHEST_IRQ_PRIORITY);		

            if (SAMPLE_RATE >= static_cast<uint32_t>(ADC_FREQUENCIES::FS_MAX_PICO)) {
                adc_set_clkdiv(0); // Reload without interval, 500 MHz.
            } else {
                adc_set_clkdiv(clk_divider);		
            }
            
            init_flag = true;
        }
    }

    // Execute only from RAM, because of critical timing: 
    static void __not_in_flash_func(adc_handler)(void) {
        if (! adc_fifo_is_empty()) {
            adc_sample_buffer[2] = adc_fifo_get();
            adc_sample_buffer[1] = adc_fifo_get();
            adc_sample_buffer[0] = adc_fifo_get();
        }
    }
};

};
#endif // ADC_DRIVER_HPP