#include <cstdio>
#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"

#include <adc_driver_dma.hpp>
#include <microphone_sensor.hpp>
#include "arm_math.h"
#include "adc_sample_distributor.hpp"
#include "locator.hpp"
#include "SampleProcessor.h"

int main() {
	/* Setup */
	stdio_init_all();
    for (;;) {}
	return 1;
}