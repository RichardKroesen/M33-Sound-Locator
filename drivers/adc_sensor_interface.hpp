#ifndef ADC_SENSOR_INTERFACE_HPP
#define ADC_SENSOR_INTERFACE_HPP

#include <cstddef>
#include <cstdint>

namespace ADC {
class ISensor {
public:
    virtual ~ISensor() = default;

    virtual uint8_t get_channel() const = 0;
    virtual void add_samples(const uint16_t* samples, size_t count) = 0;
};
} // SENSOR Namespace
#endif /* ADC_SENSOR_INTERFACE_HPP */