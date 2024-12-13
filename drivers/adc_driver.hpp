#ifndef ADC_DRIVER_HPP
#define ADC_DRIVER_HPP

#include <math.h>
#include "hardware/gpio.h"
#include "hardware/adc.h"

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
            adc_run(true);
            return true;
        }
        return false; 
    }

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
    // 48 MHz / DIV = Frequency.
    constexpr static inline uint32_t clk_divider = std::round(48'000'000 / SAMPLE_RATE);

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

            adc_set_clkdiv(clk_divider);		
            init_flag = true;
        }
    }

    // Execute only from RAM, because of critical timing: 
    static void __not_in_flash_func(adc_handler)(void) {
        if (! adc_fifo_is_empty()) {
            adc_value = adc_fifo_get();
        }
    }
};

enum class ADC_FREQUENCIES : uint32_t {
    FS_10k      = 10'000,
    FS_100k     = 100'000,
    FS_MEGA_1   = 1'000'000,

    FS_MAX_PICO = 48'000'000,
}; 
};
#endif // ADC_DRIVER_HPP