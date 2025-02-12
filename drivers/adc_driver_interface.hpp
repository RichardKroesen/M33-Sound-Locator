/** MIT License
 * 
 * Copyright (c) 2025 Feb. by Julian Bruin, James Schutte, Richard Kroesen
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef ADC_DRIVER_INTERFACE_HPP
#define ADC_DRIVER_INTERFACE_HPP

#include <math.h>

namespace ADC {

template<const ADC_FREQUENCIES SAMPLE_RATE, 
uint32_t CHANNEL_BUFFER_SAMPLES_SIZE,
uint32_t BATCH_BUFFER_SIZE = CHANNEL_BUFFER_SAMPLES_SIZE,
const uint8_t CHANNELS = 3>

class IADC_Driver {
public: 
    virtual ~IADC_Driver() = default;

    virtual const bool start_adc() = 0;
    virtual void stop_adc() = 0;
    virtual void set_notification_task(const TaskHandle_t task) = 0;
    virtual void copy_buffer(std::unique_ptr<uint16_t[]>& destination, const size_t size) = 0;

protected: 
    constexpr static inline uint32_t ADC_CYCLES_REQUIRED = 96;
    constexpr static inline uint32_t ADC_CLK_RATE = 48'000'000;
    constexpr static inline uint8_t pins_amount = CHANNELS; 

    typedef struct {
        const uint8_t pin;
        const uint8_t adc_reference;
    } pin_reference_t;

    constexpr inline static pin_reference_t pin_references[pins_amount] = {
        {26, 0},    /* GPIO26 --> ADC_CHANNEL 0*/
        {27, 1},    /* GPIO27 --> ADC_CHANNEL 1*/
        {28, 2}     /* GPIO28 --> ADC_CHANNEL 2*/
    };

    const static inline constexpr uint32_t compute_clk_divider() {
        // 48 MHz/96 Cycles / DIV = Sampling Frequency.
        // Sampling_Fs * DIV = 48MH/96
        uint32_t divider_value 
            = std::round((ADC_CLK_RATE / ADC_CYCLES_REQUIRED) / static_cast<uint32_t>(SAMPLE_RATE));
        divider_value = std::round(divider_value * CHANNELS);
        return divider_value;
    } 
}; // IADC_Driver Abstract Class
}; // namespace ADC
#endif /* ADC_DRIVER_INTERFACE_HPP */