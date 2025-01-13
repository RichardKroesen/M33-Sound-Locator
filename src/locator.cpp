
#include "locator.hpp"

#include <cmath>
#include <Eigen/Dense>

using Eigen::Matrix2d;
using Eigen::Vector2d;

const double SPEED_OF_SOUND = 340.29;
const double MAX_ITERATIONS = 100;
const double TOLERANCE = 1e-6;

/**
 * Possible optimisations:
 *  - Precaculate the delta time -> distance in meters
 *  - Precalculate the offsets (x - sensor.x) and it's y equivelant
 */

/**
 * A single iteration pass of the equation, aka, what happens if we were to fill in the equation
 * with our current guess?
 */
Vector2d equation_pass(const Vector2d& input, Algorithm::sorted_sensors_t sensors) {
    double x = input[0];
    double y = input[1];

    double distance_12 = sensors[1].duration * SPEED_OF_SOUND;
    double distance_13 = sensors[2].duration * SPEED_OF_SOUND;

    double eq1 =
        sqrt(pow(x - sensors[1].position.x, 2) + pow(y - sensors[1].position.y, 2)) -
        sqrt(pow(x - sensors[0].position.x, 2) + pow(y - sensors[0].position.y, 2)) -
        distance_12;

    double eq2 =
        sqrt(pow(x - sensors[2].position.x, 2) + pow(y - sensors[2].position.y, 2)) -
        sqrt(pow(x - sensors[0].position.x, 2) + pow(y - sensors[0].position.y, 2)) -
        distance_13;

    Vector2d result(eq1, eq2);
    return result;
}

/**
 * Same as `equation_pass` but calculated with the derivative of our equations. This is so we can
 * adjust our input to get closer to our wanted result.
 */
Matrix2d jacobian_pass(const Vector2d& input, Algorithm::sorted_sensors_t sensors) {
    double x = input[0];
    double y = input[1];

    double eq11 = 
        (x - sensors[1].position.x) /
            sqrt(
                pow(x - sensors[1].position.x, 2) + pow(y - sensors[1].position.y, 2)
            ) -
        (x - sensors[0].position.x) /
            sqrt(
                pow(x - sensors[0].position.x, 2) + pow(y - sensors[0].position.y, 2)
            );
    double eq12 = 
        (y - sensors[1].position.y) /
            sqrt(
                pow(x - sensors[1].position.x, 2) + pow(y - sensors[1].position.y, 2)
            ) -
        (y - sensors[0].position.y) /
            sqrt(
                pow(x - sensors[0].position.x, 2) + pow(y - sensors[0].position.y, 2)
            );

    double eq21 =
        (x - sensors[2].position.x) /
            sqrt(
                pow(x - sensors[2].position.x, 2) + pow(y - sensors[2].position.y, 2)
            ) -
        (x - sensors[0].position.x) /
            sqrt(
                pow(x - sensors[0].position.x, 2) + pow(y - sensors[0].position.y, 2)
            );
    double eq22 =
        (y - sensors[2].position.y) /
            sqrt(
                pow(x - sensors[2].position.x, 2) + pow(y - sensors[2].position.y, 2)
            ) -
        (y - sensors[0].position.y) /
            sqrt(
                pow(x - sensors[0].position.x, 2) + pow(y - sensors[0].position.y, 2)
            );

    Matrix2d result;
    result(0, 0) = eq11;
    result(0, 1) = eq12;
    result(1, 0) = eq21;
    result(1, 1) = eq22;
    return result;
}

namespace Algorithm {
    /**
     * TYPE SAVETY:
     * This mutates the sensors array!
     */
    void sort_sensors(sensor_t sensors[3]) {
        // Since our input array is just 3 long, don't even bother with a sorting algorithm
        // Just manually sort
        if (sensors[0].duration > sensors[1].duration) {
            sensor_t temp = sensors[0];
            sensors[0] = sensors[1];
            sensors[1] = temp;
        }
        if (sensors[1].duration > sensors[2].duration) {
            sensor_t temp = sensors[1];
            sensors[1] = sensors[2];
            sensors[2] = temp;
        }

        // Get the delta timings
        sensors[2].duration -= sensors[0].duration;
        sensors[1].duration -= sensors[0].duration;
        sensors[0].duration = 0;
    }

    vec2_t get_sensors_mean(sensor_t sensors[3]) {
        double mean_x = (
            sensors[0].position.x +
            sensors[1].position.x +
            sensors[2].position.x
        ) / 3;
        double mean_y = (
            sensors[0].position.y +
            sensors[1].position.y +
            sensors[2].position.y
        ) / 3;
        
        vec2_t result;
        result.x = mean_x;
        result.y = mean_y;
        
        return result;
    }

    bool calc_position(sensor_t sensors[3], vec2_t* out_position) {
        sort_sensors(sensors);
        vec2 mean = get_sensors_mean(sensors);

        Vector2d input(mean.x, mean.y);
        for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
             Vector2d output = equation_pass(input, sensors);
             Matrix2d derivative = jacobian_pass(input, sensors);

             Vector2d adjust = derivative.colPivHouseholderQr().solve(-output);
             input += adjust;

             if (adjust.norm() < TOLERANCE) {
                 out_position->x = input[0];
                 out_position->y = input[1];
                 return true;
             }
        }
        return false;
    }
}

