#ifndef BUZZER_HPP
#define BUZZER_HPP

#include <math.h>
#include <cstdint>
#include <stdint.h>
#include "hardware/pwm.h"

namespace ACTUATOR {
template <uint8_t PIN> 
class Buzzer {
public: 
    Buzzer() {
        init();
    }

    ~Buzzer() = default;

    static inline void start_buzzer(const uint32_t duration_ms) {
        pwm_set_enabled(slice_num, true);
        pwm_set_wrap(slice_num, complete_slice);
        pwm_set_chan_level(slice_num, PWM_CHAN_B, 40);
        add_alarm_in_ms(duration_ms, single_shot_timer_callback, NULL, false);
    }

    static inline void stop_buzzer() {
        buzzer_determinate_flag = false;
        pwm_set_enabled(slice_num, false);
    }

private:
    constexpr static inline uint8_t clk_divider = 200;
    constexpr static inline uint32_t buzzer_frequency = 500;
    constexpr static inline uint32_t complete_slice = 1500;
    constexpr static inline uint32_t duty_cycle = 50; 

    static inline bool init_flag = false;
    static inline uint8_t slice_num = 0;
    static inline volatile bool buzzer_determinate_flag = false;

    static inline bool init() {
        if (! init_flag) {
            constexpr uint32_t cycle_high = 40; // (std::round(complete_slice/100 * duty_cycle)) + 1;
            gpio_set_function(PIN, GPIO_FUNC_PWM);
            slice_num = pwm_gpio_to_slice_num(PIN);
            
            pwm_set_clkdiv(slice_num, static_cast<float>(clk_divider));
            pwm_set_wrap(slice_num, complete_slice);
            pwm_set_chan_level(slice_num, PWM_CHAN_B, cycle_high);
            pwm_set_enabled(slice_num, true);

            init_flag = true; 
            return true;
        }
        return false; 
    }

    static inline int64_t single_shot_timer_callback(alarm_id_t id, __unused void *user_data) {
        pwm_set_enabled(slice_num, false);
        return 0; // The timer is cancelled after this. 
    }
}; // Class Buzzer 
} // Namespace ACTUATOR
#endif /* BUZZER_HPP */