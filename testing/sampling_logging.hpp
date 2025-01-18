#include <adc_driver_dma.hpp>
#include <microphone_sensor.hpp>
#include "adc_sample_distributor.hpp"

void sampling_print_task(void *param) {
    constexpr uint32_t post_processing_buffer_size = 1000;
    using Microphone0 = SENSOR::Microphone<0, post_processing_buffer_size>;
    auto mic0 = std::make_shared<Microphone0>();
    
    using Microphone1 = SENSOR::Microphone<1, post_processing_buffer_size>;
    auto mic1 = std::make_shared<Microphone1>();

    using Microphone2 = SENSOR::Microphone<2, post_processing_buffer_size>;
    auto mic2 = std::make_shared<Microphone2>();

    constexpr ADC_FREQUENCIES FS = ADC_FREQUENCIES::FS_MAX_PICO;
    constexpr uint32_t total_buffer_size = 60;
    constexpr uint32_t batch_buffer_size = 12;

    static ADC::ADC_Driver_DMA<FS, total_buffer_size, batch_buffer_size> adc_driver;
    static std::unique_ptr<uint16_t[]> buffer = std::make_unique<uint16_t[]>(batch_buffer_size);

    ADC::SampleDistributor<ADC::ISensor, 3> distributor({mic0, mic1, mic2});

    adc_driver.set_notification_task(xTaskGetCurrentTaskHandle());
    adc_driver.start_adc();

    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        adc_driver.copy_buffer(buffer, batch_buffer_size);

        for (uint32_t i = 0; i < batch_buffer_size-2; i = i + 3) {
            printf("%u,%u,%u\n", buffer[i], buffer[i+1], buffer[i+2]);
        }

        distributor.distribute_samples<batch_buffer_size>(buffer);
        adc_driver.start_adc();
    }
}
