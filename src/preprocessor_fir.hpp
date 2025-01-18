#ifndef PREPROCESSOR_FIR_HPP
#define PREPROCESSOR_FIR_HPP

#include <arm_math.h>
#include "preprocessor_interface.hpp"

namespace PROCESSING {

template <const size_t NUM_TAPS, const size_t BLOCK_SIZE>
class FIRFilter : public IPreprocessor {
public:
    explicit FIRFilter(const float32_t (&coefficients)[NUM_TAPS]) : coeffs(coefficients) {}

    void process(const uint16_t* input, uint16_t* output, size_t count) const override;

private:
    const float32_t (&coeffs)[NUM_TAPS];
};

} // Namespace PROCESSING
#endif /* PREPROCESSOR_FIR_HPP */