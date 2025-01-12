#pragma once

namespace Algorithm {
    typedef struct vec2 {
        double x;
        double y;
    } vec2_t;

    typedef struct sensor {
        vec2_t position;
        double duration;
        bool measured;
    } sensor_t;

    typedef sensor_t* sorted_sensors_t;

    bool calc_position(sensor_t sensors[3], vec2_t* out_position);
}

