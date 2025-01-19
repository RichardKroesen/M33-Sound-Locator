#ifndef BOARD_DEFINES_H
#define BOARD_DEFINES_H

#include <cstdint>

/* PIN DEFINITIONS WILL GO HERE */
namespace UTILS {
    
class PinManager {
public:
    struct GPIO {
        static constexpr inline uint8_t BUZ_ACTIVATE    = 1;
        static constexpr inline uint8_t BUZ_OUTPUT      = 2;
        /* Reserved for System State Indicators */
    };

    struct ADC {
        static constexpr inline uint8_t CH0 = 26;   /* GPIO26 --> ADC_CHANNEL 0*/
        static constexpr inline uint8_t CH1 = 27;   /* GPIO27 --> ADC_CHANNEL 1*/
        static constexpr inline uint8_t CH2 = 28;   /* GPIO28 --> ADC_CHANNEL 2*/
    };

    struct COMM {
        /* Reserved... */
    };

    // Maybe some default configs: 
    // static void configurePins() {
    // }

};
} // namespace UTILS 
#endif /* BOARD_DEFINES_H */