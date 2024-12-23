#ifndef ADC_DMA_DRIVER_HPP
#define ADC_DMA_DRIVER_HPP

#include <math.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "system_references.hpp"

namespace ADC {

template<const ADC_FREQUENCIES SAMPLE_RATE, 
const uint8_t CHANNELS = 3, 
uint32_t CHANNEL_BUFFER_SAMPLES_SIZE = 300>

class ADC_Driver_DMA {
public: 
    ADC_Driver_DMA() {
        static_assert(static_cast<uint32_t>(SAMPLE_RATE) > 0, "Err sample rate cannot be 0.");
        static_assert(CHANNELS < 5, "Err not possible to select more than 4 channels.");
    }

    ~ADC_Driver_DMA() {
        if (dma_channel >= 0) {
            dma_channel_unclaim(dma_channel);
        }
    }

    const static inline bool start_adc() {
        init();
        if (init_flag) {
            // Clear the DMA Buffer;
            adc_run(false);
            adc_fifo_drain();

            memset(dma_buffer, 0, sizeof(dma_buffer));
            adc_select_input(pinReferences[0].adc_reference); // Initial Channel. 

            dma_channel_config c = dma_channel_get_default_config(dma_channel);
            channel_config_set_transfer_data_size(&c, DMA_SIZE_16); // Sample is 2 byte.
            channel_config_set_read_increment(&c, false);
            channel_config_set_write_increment(&c, true);
            channel_config_set_dreq(&c, DREQ_ADC);

            dma_channel_configure(
                dma_channel,                            // Channel
                &c,                                     // Configuration
                dma_buffer,                             // Destination buffer
                &adc_hw->fifo,                          // Source is ADC FIFO
                CHANNEL_BUFFER_SAMPLES_SIZE * CHANNELS, // Transfer size
                true                                    // start immediately
            );

            adc_run(true);
            return true;
        }
        return false; 
    }

    static inline void stop_adc() {
        adc_run(false);
    }

    static inline void set_notification_task(TaskHandle_t task) {
        notificationTask = task;
    }

    static inline void get_buffer(uint16_t *buf) {
        vPortEnterCritical();
        memcpy(buf, dma_buffer, sizeof(dma_buffer));
        vPortExitCritical();
    }

protected: 
    constexpr static inline uint32_t ADC_CYCLES_REQUIRED = 96;
    constexpr static inline uint32_t ADC_CLK_RATE = 48'000'000;
    constexpr static inline uint8_t pins_amount = CHANNELS; 

    static inline uint16_t dma_buffer[CHANNEL_BUFFER_SAMPLES_SIZE * CHANNELS] = {};
    static inline volatile int8_t dma_channel = {-1};

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
        uint32_t divider_value = std::round((ADC_CLK_RATE / ADC_CYCLES_REQUIRED) / static_cast<uint32_t>(SAMPLE_RATE));
        divider_value = std::round(divider_value * CHANNELS);
        return divider_value;
    } 

private: 
    constexpr static inline uint32_t clk_divider = compute_clk_divider();
    static inline bool init_flag = false;

    static inline TaskHandle_t notificationTask = nullptr;

    static inline void init() {
        if (! init_flag) {            
            uint16_t adc_channels_mask = 0x0000;
            configASSERT(notificationTask != nullptr);

            adc_init();
            adc_run(false);

            for (uint8_t i = 0; i < CHANNELS; i++) {
                gpio_set_dir(pinReferences[i].pin, false);
                gpio_disable_pulls(pinReferences[i].pin);
                adc_gpio_init(pinReferences[i].pin);
            }

            for (uint8_t i = 0; i < CHANNELS; i++) {
                adc_channels_mask |= (1 << pinReferences[i].adc_reference);
            }

            adc_set_round_robin(adc_channels_mask);
            adc_fifo_setup(
                true,       // Enable FIFO
                true,       // DMA Enabled
                CHANNELS,   // Threshold value, RP2350 can only hold 8 samples in FIFO
                false,       // No error interrupts
                false       // No threshold interrupts
            );

            if (SAMPLE_RATE >= (ADC_FREQUENCIES::FS_MAX_PICO)) {
                adc_set_clkdiv(0); // Reload without interval, 500 MHz.
            } else {
                adc_set_clkdiv(clk_divider);
            }

            dma_channel = dma_claim_unused_channel(true);
            configASSERT(dma_channel >= 0);

            dma_channel_config c = dma_channel_get_default_config(dma_channel);
            channel_config_set_transfer_data_size(&c, DMA_SIZE_16); // Sample is 2 byte.
            channel_config_set_read_increment(&c, false);
            channel_config_set_write_increment(&c, true);
            channel_config_set_dreq(&c, DREQ_ADC);

            dma_channel_configure(
                dma_channel,                            // Channel
                &c,                                     // Configuration
                dma_buffer,                             // Destination buffer
                &adc_hw->fifo,                          // Source is ADC FIFO
                CHANNEL_BUFFER_SAMPLES_SIZE * CHANNELS, // Transfer size
                true                                    // start immediately
            );

            // Set IRQ handler
            dma_channel_set_irq0_enabled(dma_channel, true);
            irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
            irq_set_enabled(DMA_IRQ_0, true);

            init_flag = true;
        }
    }

    static void dma_irq_handler() {
        if (! dma_channel_is_busy(dma_channel)) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            vTaskNotifyGiveFromISR(notificationTask, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            dma_hw->ints0 = 1u << dma_channel;
        }
    }

}; // ADC_DMA_Driver Class
}; // ADC Namespace
#endif /* ADC_DMA_DRIVER_HPP */