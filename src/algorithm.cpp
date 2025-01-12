
#include "algorithm.hpp"

#include <cmath>
#include <Eigen/Dense>

const double SPEED_OF_SOUND = 340.29;

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
}

