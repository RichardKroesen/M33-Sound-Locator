#ifndef SYSTEM_REFERENCES_HPP
#define SYSTEM_REFERENCES_HPP

#include <stdint.h>

enum class ADC_FREQUENCIES : uint32_t {
    FS_100hz    = 100,
    FS_1k       = 1'000,
    FS_10k      = 10'000,
    FS_100k     = 100'000,

    FS_MAX_PICO = 500'000,
}; 

#endif /* SYSTEM_REFERENCES_HPP */