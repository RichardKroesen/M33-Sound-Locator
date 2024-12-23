#ifndef ADC_DRIVER__IRQ_HPP
#define ADC_DRIVER__IRQ_HPP

#include <math.h>
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "system_references.hpp"
#include <adc_driver_interface.hpp>

namespace ADC {

template<const ADC_FREQUENCIES SAMPLE_RATE = ADC_FREQUENCIES::FS_100hz, 
const uint8_t CHANNELS = 3, 
uint32_t CHANNEL_BUFFER_SAMPLES_SIZE = 630>

class ADC_Driver : public IADC_Driver<SAMPLE_RATE, CHANNELS, CHANNEL_BUFFER_SAMPLES_SIZE> {
    
    using IADC = IADC_Driver<SAMPLE_RATE, CHANNELS, CHANNEL_BUFFER_SAMPLES_SIZE>; // Alias for convenience

public: 
    ADC_Driver() {
        static_assert(static_cast<uint32_t>(SAMPLE_RATE) > 0, "Err sample rate cannot be 0");
        static_assert(CHANNELS < 5, "Err not possible to select more than 4 channels.");
        init();
    }

    ~ADC_Driver() {
        if (stream_buffer != nullptr) {
            vStreamBufferDelete(stream_buffer);
        }
    }

    const bool start_adc() override {
        return start_adc_static();
    }

    void stop_adc() override {
        stop_adc_static();
    }

    static inline void set_notification_task(TaskHandle_t task) {
        notificationTask = task;
    }

    // static inline void get_buffer(uint16_t *buf) {        
    //     vPortEnterCritical();
    //     xStreamBufferReceive(stream_buffer, buf, sizeof(stream_buffer), 0);
    //     vPortExitCritical();
    // }

    bool readSamples(uint16_t* buffer, size_t count) {
        vPortEnterCritical();
        size_t bytesRead = xStreamBufferReceive(stream_buffer, buffer, count * sizeof(uint16_t), 0);
        vPortExitCritical();
        return (bytesRead == count * sizeof(uint16_t));
    }
    
protected:
    const static inline bool start_adc_static() {
        if (init_flag) {
            adc_run(false);
            adc_fifo_drain();

            adc_select_input(IADC::pin_references[0].adc_reference); // Initial Channel. 
            adc_irq_set_enabled(true);
            irq_set_enabled(ADC_IRQ_FIFO, true);
            adc_run(true);
            return true;
        }
        return false; 
    }

    static inline void stop_adc_static() {
        adc_run(false);
        adc_fifo_drain();
        
        adc_irq_set_enabled(false);
        irq_set_enabled(ADC_IRQ_FIFO, false);

        xStreamBufferReset(stream_buffer);
    }
    
private: 
    constexpr static size_t STREAM_BUFFER_B_SIZE  = CHANNEL_BUFFER_SAMPLES_SIZE * sizeof(uint16_t); // Size in bytes
    constexpr static size_t TRIGGER_LEVEL = 201;                                       // Minimum bytes to wake a task
    constexpr static inline uint32_t clk_divider = IADC::compute_clk_divider();

    static inline StreamBufferHandle_t stream_buffer = {nullptr};
    static inline TaskHandle_t notificationTask = nullptr;

    static inline bool init_flag = false;

    static inline void init() {
        if (! init_flag) {
            uint16_t adc_channels_mask = 0x0000;
            adc_init();

            stream_buffer = xStreamBufferCreate(STREAM_BUFFER_B_SIZE, TRIGGER_LEVEL);
            configASSERT(stream_buffer != nullptr);

            for (size_t i = 0; i < CHANNELS; i++) {
                gpio_set_dir(IADC::pin_references[i].pin, false);
                gpio_disable_pulls(IADC::pin_references[i].pin);
                adc_gpio_init(IADC::pin_references[i].pin);
            }

            for (uint8_t i = 0; i < CHANNELS; i++) {
                adc_channels_mask |= (1 << IADC::pin_references[i].adc_reference);
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

            if (SAMPLE_RATE >= (ADC_FREQUENCIES::FS_MAX_PICO)) {
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
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            for (uint8_t i = 0; i < CHANNELS; i++) {
                uint16_t sample = adc_fifo_get();
                xStreamBufferSendFromISR(stream_buffer, 
                    &sample, sizeof(uint16_t), &xHigherPriorityTaskWoken);
            }
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    } 
}; // ADC_DRIVER Class
}; // ADC Namespace
#endif /* ADC_DRIVER__IRQ_HPP */