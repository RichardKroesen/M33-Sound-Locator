#ifndef ADC_DRIVER_HPP
#define ADC_DRIVER_HPP

#include <math.h>
#include "stream_buffer.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "system_references.hpp"

namespace ADC {

template<const uint32_t SAMPLE_RATE, 
const uint8_t CHANNELS = 3, uint32_t CHANNEL_BUFFER_SAMPLES_SIZE = 250>

class ADC_Driver {
    constexpr static inline uint8_t pins_amount = CHANNELS; 

public: 
    ADC_Driver() {
        static_assert(SAMPLE_RATE > 0, "Err sample rate cannot be 0");
        static_assert(CHANNELS < 5, "Err not possible to select more than 4 channels.");
        init();
    }

    ~ADC_Driver() {
        if (stream_buffer != nullptr) {
            vStreamBufferDelete(stream_buffer);
        }
    }

    const static inline bool start_adc() {
        if (init_flag) {
            adc_select_input(pinReferences[0].adc_reference); // Initial Channel. 
            adc_irq_set_enabled(true);
            irq_set_enabled(ADC_IRQ_FIFO, true);
            
            adc_fifo_drain();
            adc_run(true);
            return true;
        }
        return false; 
    }

    static inline void stop_adc() {
        adc_run(false);
        adc_irq_set_enabled(false);
        irq_set_enabled(ADC_IRQ_FIFO, false);
        xStreamBufferReset(stream_buffer);
        adc_fifo_drain();
    }

    bool readSamples(uint16_t* buffer, size_t count) {
        size_t bytesRead = xStreamBufferReceive(stream_buffer, buffer, count * sizeof(uint16_t), 0);
        return (bytesRead == count * sizeof(uint16_t));
    }
    
protected: 
    constexpr static inline uint32_t ADC_CYCLES_REQUIRED = 96;
    constexpr static inline uint32_t ADC_CLK_RATE = 48'000'000;

    static inline StreamBufferHandle_t stream_buffer = nullptr;

    typedef struct {
        const uint8_t pin;
        const uint8_t adc_reference;
    } pin_reference_t;


    constexpr inline static pin_reference_t pinReferences[pins_amount] = {
        {26, 0},    /* GPIO26 --> ADC_CHANNEL 0*/
        {27, 1},    /* GPIO27 --> ADC_CHANNEL 1*/
        {28, 2}     /* GPIO28 --> ADC_CHANNEL 2*/
    };

    const static inline constexpr uint32_t compute_clk_divider() {
        // 48 MHz/96 Cycles / DIV = Sampling Frequency.
        uint32_t divider_value = std::round((ADC_CLK_RATE / ADC_CYCLES_REQUIRED) / SAMPLE_RATE);
        divider_value = std::round(divider_value * CHANNELS);
        return divider_value;
    } 

private: 
    constexpr static size_t STREAM_BUFFER_B_SIZE 
        = CHANNELS * CHANNEL_BUFFER_SAMPLES_SIZE * sizeof(uint16_t);        // Size in bytes
    constexpr static size_t TRIGGER_LEVEL = CHANNELS * sizeof(uint16_t);    // Minimum bytes to wake a task
    constexpr static inline uint32_t clk_divider = compute_clk_divider();

    static inline bool init_flag = false;

    static inline void init() {
        if (! init_flag) {
            uint16_t adc_channels_mask = 0x0000;
            stream_buffer = xStreamBufferCreate(STREAM_BUFFER_B_SIZE, TRIGGER_LEVEL);
            configASSERT(stream_buffer != nullptr);

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
        static uint16_t inter_buffer[CHANNELS]{};

        if (! adc_fifo_is_empty()) {
            for (int8_t i = 0; i < CHANNELS; i++) {
                inter_buffer[i] = adc_fifo_get();
            }
            xStreamBufferSendFromISR(stream_buffer, inter_buffer, sizeof(inter_buffer), nullptr);
        }
    }
}; // ADC_DRIVER Class
}; // ADC Namespace
#endif /* ADC_DRIVER_HPP */