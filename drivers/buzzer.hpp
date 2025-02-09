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

#ifndef BUZZER_HPP
#define BUZZER_HPP

#include <math.h>
#include <cstdint>
#include <stdint.h>
#include "hardware/pwm.h"

namespace ACTUATOR {

template <const uint8_t PIN> 
class Buzzer {
public: 
    Buzzer() {
        init();
    }

    ~Buzzer() = default;

    static inline void start_buzzer(const uint32_t duration_ms) {
        if (! init_flag) {
            init();
        }
        
        pwm_set_enabled(slice_num, true);
        buzzer_terminated_flag = false;
        add_alarm_in_ms(duration_ms, single_shot_timer_callback, NULL, false);
    }

    static inline void stop_buzzer() {
        buzzer_terminated_flag = true;
        pwm_set_enabled(slice_num, false);
    }

    const static inline bool get_state() {
        return buzzer_terminated_flag;
    }

    static inline void set_tone_frequency(const uint32_t ft) {
        if (! buzzer_frequency) {
            stop_buzzer();
            return;
        }

        init_flag = false; 
        buzzer_frequency = ft; 
    }

    static inline void set_volume(const uint8_t percentage) {
        duty_cycle = percentage;
        init_flag = false; 
    }

private:
    constexpr static inline uint8_t clk_divider = 200;
    constexpr static inline uint32_t pwm_clock = (150'000'000/clk_divider);
    static inline uint32_t buzzer_frequency = 500;
    static inline uint32_t duty_cycle = 50; 

    static inline uint16_t wrap_cycle = 100;
    static inline uint32_t cycle_high = 50; 

    static inline bool init_flag = false;
    static inline uint8_t slice_num = 0;
    static inline volatile bool buzzer_terminated_flag = true;

    static inline bool init() {
        assert(buzzer_frequency > 0 && "Error, buzzer must be greater than 0.");

        if (! init_flag) {
            wrap_cycle = (pwm_clock/buzzer_frequency);
            cycle_high = round((wrap_cycle/100)) * duty_cycle;

            gpio_set_function(PIN, GPIO_FUNC_PWM);
            slice_num = pwm_gpio_to_slice_num(PIN);
            
            pwm_set_clkdiv(slice_num, static_cast<float>(clk_divider));
            pwm_set_wrap(slice_num, wrap_cycle);
            
            if constexpr ((PIN % 2) == 0) {
                /* Even Pins must select CHAN_A */
                pwm_set_chan_level(slice_num, PWM_CHAN_A, cycle_high + 1);
            } else {
                pwm_set_chan_level(slice_num, PWM_CHAN_B, cycle_high + 1);
            }

            pwm_set_enabled(slice_num, false);

            init_flag = true; 
            return true;
        }
        return false; 
    }

    static inline int64_t single_shot_timer_callback(alarm_id_t id, __unused void *user_data) {
        pwm_set_enabled(slice_num, false);
        buzzer_terminated_flag = true;
        return 0; // The timer is cancelled after this. 
    }
}; // Class Buzzer 
} // Namespace ACTUATOR
#endif /* BUZZER_HPP */