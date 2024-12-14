#ifndef ADC_DRIVER_HPP
#define ADC_DRIVER_HPP

#include <math.h>
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "system_references.hpp"

namespace DRIVER {

template<const uint32_t SAMPLE_RATE, const uint8_t CHANNELS = 3>
class ADC_Driver {
    constexpr static inline uint8_t pins_amount = CHANNELS; 

public: 
    ADC_Driver() {
        static_assert(CHANNELS < 5, "Err not possible to select than 4 channels.");
        init();
    }

    ~ADC_Driver() = default; 

    const static inline bool start_adc() {
        if (init_flag) {
            adc_select_input(pinReferences[0].adc_reference); // Initial Channel. 
            adc_irq_set_enabled(true);
            irq_set_enabled(ADC_IRQ_FIFO, true);

            for (uint8_t i = 0; i < CHANNELS; i++) {
                adc_sample_buffer[i] = 0;
            }
            
            // adc_fifo_drain();
            adc_run(true);
            return true;
        }
        return false; 
    }

    static inline void stop_adc() {
        adc_run(false);
        adc_irq_set_enabled(false);
        irq_set_enabled(ADC_IRQ_FIFO, false);
        adc_fifo_drain();
    }

    // I will differ this, since this will probably change due to buffering need...
    // static inline bool read_sample(const uint8_t channel, uint16_t &value) {
    //     if (channel < 3) {
    //         taskENTER_CRITICAL();
    //         value = adc_sample_buffer;
    //         taskEXIT_CRITICAL();
    //         return true;
    //     }
    //     return false;
    // }

    static inline volatile uint16_t adc_sample_buffer[pins_amount] = {0, 0, 0}; // Will become a buffer...

protected: 
    typedef struct {
        const uint8_t pin;
        const uint8_t adc_reference;
    } pin_reference_t;

    constexpr inline static pin_reference_t pinReferences[pins_amount] = {
        {26, 0},
        {27, 1},
        {28, 2}
    };

    const static inline constexpr uint32_t compute_divider() {
        uint32_t divider_value = std::round((48'000'000/96) / SAMPLE_RATE);

        if constexpr (CHANNELS != 0) {
            divider_value = std::round(divider_value * CHANNELS);
        }
        return divider_value;
    } 

private: 
    static inline bool init_flag = false;
    // 48 MHz/96 Cycles / DIV = Sampling Frequency.
    constexpr static inline uint32_t clk_divider = compute_divider();

    static inline void init() {
        if (! init_flag) {
            uint16_t adc_channels_mask = 0x0000;
            adc_init();
            
            for (auto &p : pinReferences) {
                gpio_set_dir(p.pin, false);
                gpio_disable_pulls(p.pin);
                adc_gpio_init(p.pin);
            }

            for (uint8_t i = 0; i < CHANNELS; i++) {
                adc_channels_mask |= (1 << pinReferences[i].adc_reference);
            }

            adc_set_round_robin(adc_channels_mask);
            adc_fifo_setup(
                true,
                false,
                CHANNELS,
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
            for (int8_t i = CHANNELS-1; i >= 0; i--) {
                adc_sample_buffer[i] = adc_fifo_get();
            }
        }
    }
};

};
#endif // ADC_DRIVER_HPP