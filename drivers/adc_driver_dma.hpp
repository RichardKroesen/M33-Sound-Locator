#ifndef ADC_DMA_DRIVER_HPP
#define ADC_DMA_DRIVER_HPP

#include <math.h> // rounding
#include <memory> // Smarpointer
#include <iterator> // iterators
#include <cstring>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/dma.h"

#include "system_references.hpp"
#include "adc_driver_interface.hpp"

namespace ADC {

template<const ADC_FREQUENCIES SAMPLE_RATE, 
uint32_t CHANNEL_BUFFER_SAMPLES_SIZE = 300,
uint32_t BATCH_BUFFER_SIZE = CHANNEL_BUFFER_SAMPLES_SIZE,
const uint8_t CHANNELS = 3>

class ADC_Driver_DMA : public IADC_Driver<SAMPLE_RATE, CHANNEL_BUFFER_SAMPLES_SIZE, BATCH_BUFFER_SIZE, CHANNELS> {

    using IADC = IADC_Driver<SAMPLE_RATE, CHANNEL_BUFFER_SAMPLES_SIZE, BATCH_BUFFER_SIZE, CHANNELS>; // Alias for convenience

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

    const bool start_adc() override {
        return start_adc_static(); 
    }

    void stop_adc() override {
        stop_adc_static();
    }

    void set_notification_task(const TaskHandle_t task) override {
        this->notificationTask = task;
    }

    void copy_buffer(std::unique_ptr<uint16_t[]>& destination, const size_t size) override {
        configASSERT(size <= sizeof(dma_buffer) / sizeof(dma_buffer[0])); 
        taskENTER_CRITICAL(); 
        std::copy(std::begin(dma_buffer), std::begin(dma_buffer) + size, destination.get());
        taskEXIT_CRITICAL();
    }

protected:
    const static inline bool start_adc_static() {
        init();
        if (init_flag) {
            adc_run(false);
            adc_fifo_drain();

            memset(dma_buffer, 0, sizeof(dma_buffer));
            adc_select_input(IADC::pin_references[0].adc_reference); // Initial Channel. 
            config_dma();

            adc_run(true);
            return true;
        }
        return false; 
    }

    static inline void stop_adc_static() {
        adc_run(false);
    }

private: 
    constexpr static inline uint32_t clk_divider = IADC::compute_clk_divider();

    static inline bool init_flag = false;
    static inline TaskHandle_t notificationTask = nullptr;
    static inline uint16_t dma_buffer[CHANNEL_BUFFER_SAMPLES_SIZE * CHANNELS] = {};
    static inline volatile int8_t dma_channel = {-1};

    static inline void init() {
        if (! init_flag) {            
            uint16_t adc_channels_mask = 0x0000;
            configASSERT(notificationTask != nullptr);

            adc_init();
            adc_run(false);

            for (uint8_t i = 0; i < CHANNELS; i++) {
                gpio_set_dir(IADC::pin_references[i].pin, false);
                gpio_disable_pulls(IADC::pin_references[i].pin);
                adc_gpio_init(IADC::pin_references[i].pin);
            }

            for (uint8_t i = 0; i < CHANNELS; i++) {
                adc_channels_mask |= (1 << IADC::pin_references[i].adc_reference);
            }

            adc_set_round_robin(adc_channels_mask);
            adc_fifo_setup(
                true,       // Enable FIFO
                true,       // DMA Enabled
                CHANNELS,   // Threshold value, RP2350 can only hold 8 samples in FIFO
                false,       // No error interrupts
                false       // No byte shifts
            );

            if (SAMPLE_RATE >= (ADC_FREQUENCIES::FS_MAX_PICO)) {
                adc_set_clkdiv(0); // Reload without interval, 500 MHz.
            } else {
                adc_set_clkdiv(clk_divider);
            }

            dma_channel = dma_claim_unused_channel(true);
            configASSERT(dma_channel >= 0);
            config_dma();
            // Set IRQ handler
            dma_channel_set_irq0_enabled(dma_channel, true);
            irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
            irq_set_enabled(DMA_IRQ_0, true);

            init_flag = true;
        }
    }

    static inline void config_dma() {
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
            BATCH_BUFFER_SIZE * CHANNELS,           // Transfer size
            true                                    // start immediately
        );
    }

    static void __not_in_flash_func(dma_irq_handler)(void) {
        dma_hw->ints0 = 1u << dma_channel;
        if (! dma_channel_is_busy(dma_channel)) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            vTaskNotifyGiveFromISR(notificationTask, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }

}; // ADC_DMA_Driver Class
}; // ADC Namespace
#endif /* ADC_DMA_DRIVER_HPP */