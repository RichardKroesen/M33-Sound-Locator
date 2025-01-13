#pragma once

#include <cassert>
#include <cmath>
#include <cstddef>

#include <Eigen/Dense>

using Eigen::Matrix2d;
using Eigen::Vector2d;

namespace Locator {
    typedef struct vec2 {
        double x;
        double y;
    } vec2_t;

    typedef struct sensor {
        vec2_t position;
        double at;
        bool measured;
    } sensor_t;

    typedef sensor_t* sorted_sensors_t;

    class SensorConfig {
        private:
            sensor_t sensors[3];

            void sort() {
                // Since our input array is just 3 long, don't even bother with a sorting algorithm
                // Just manually sort
                if (this->sensors[0].at > this->sensors[1].at) {
                    sensor_t temp = this->sensors[0];
                    this->sensors[0] = this->sensors[1];
                    this->sensors[1] = temp;
                }
                if (this->sensors[1].at > this->sensors[2].at) {
                    sensor_t temp = this->sensors[1];
                    this->sensors[1] = this->sensors[2];
                    this->sensors[2] = temp;
                }
            }

            vec2_t mean_position() {
                double mean_x = (
                    this->sensors[0].position.x +
                    this->sensors[1].position.x +
                    this->sensors[2].position.x
                ) / 3;
                double mean_y = (
                    this->sensors[0].position.y +
                    this->sensors[1].position.y +
                    this->sensors[2].position.y
                ) / 3;
                
                vec2_t result;
                result.x = mean_x;
                result.y = mean_y;
                
                return result;
            }

            /**
             * Formula for determining the position to the source, given reference sensors
             * and timed distance between them
             */
            inline double distance_parabola(vec2_t guess, vec2_t sensor, vec2_t reference, double distance) {
                return 
                    sqrt(pow(guess.x - sensor.x, 2) + pow(guess.y - sensor.y, 2)) -
                    sqrt(pow(guess.x - reference.x, 2) + pow(guess.y - reference.y, 2)) -
                    distance;
            };

            inline Vector2d equation_pass(const Vector2d& input) {
                double distance_01 = (this->sensors[1].at - this->sensors[0].at) * this->speed_of_sound;
                double distance_02 = (this->sensors[2].at - this->sensors[0].at) * this->speed_of_sound;

                vec2_t input_v = vec2_t { input[0], input [1] };

                double x = this->distance_parabola(
                    input_v,
                    this->sensors[1].position, this->sensors[0].position,
                    distance_01
                );
                double y = this->distance_parabola(
                    input_v,
                    this->sensors[2].position, this->sensors[0].position,
                    distance_02
                );

                return Vector2d(x, y);
            }

            /**
             * Partial derivative of `distance_parabola`, used in the jacobian matrix pass
             */
            inline vec2_t distance_parabola_derivative(vec2_t guess, vec2_t sensor, vec2_t reference) {
                double distance_sensor = sqrt(
                    pow(guess.x - sensor.x, 2) + pow(guess.y - sensor.y, 2)
                );
                double distance_reference = sqrt(
                    pow(guess.x - reference.x, 2) + pow(guess.y - reference.y, 2)
                );
                assert(distance_sensor != 0);
                assert(distance_reference != 0);

                double x =
                    (guess.x - sensor.x) / distance_sensor -
                    (guess.x - reference.x) / distance_reference;

                double y =
                    (guess.y - sensor.y) / distance_sensor -
                    (guess.y - reference.y) / distance_reference;

                return vec2_t { x, y };
            };
            
            inline Matrix2d jacobian_pass(const Vector2d& input) {
                vec2_t input_v = vec2_t { input[0], input[1] };
                vec2_t column_0 = this->distance_parabola_derivative(
                    input_v,
                    this->sensors[1].position,
                    this->sensors[0].position
                );
                vec2_t column_1 = this->distance_parabola_derivative(
                    input_v,
                    this->sensors[2].position,
                    this->sensors[0].position
                );

                Matrix2d result;
                result(0, 0) = column_0.x;
                result(0, 1) = column_0.y;
                result(1, 0) = column_1.x;
                result(1, 1) = column_1.y;
                return result;
            }

        public:
            double speed_of_sound = 340.29;
            double max_iterations = 100;
            double tolerance = 1e-6;

            SensorConfig(vec2_t a, vec2_t b, vec2_t c) {
                this->sensors[0].position = a;
                this->sensors[1].position = b;
                this->sensors[2].position = c;
            }

            void measured(size_t index, double time) {
                assert(index >= 0 && "Index must be higher or equal to 0");
                assert(index <= 2 && "Index must be lower than 3");

                this->sensors[index].at = time;
                this->sensors[index].measured = true;
            };

            bool calculate(vec2_t* out_position) {
                assert(this->sensors[0].measured && "Sensor 0 not measured");
                assert(this->sensors[1].measured && "Sensor 1 not measured");
                assert(this->sensors[2].measured && "Sensor 2 not measured");
                this->sensors[0].measured = false;
                this->sensors[1].measured = false;
                this->sensors[2].measured = false;

                this->sort();
                vec2 mean = this->mean_position();

                Vector2d input(mean.x, mean.y);
                for (int iteration = 0; iteration < this->max_iterations; iteration++) {
                    Vector2d output = this->equation_pass(input);
                    Matrix2d derivative = this->jacobian_pass(input);

                    Vector2d adjust = derivative.colPivHouseholderQr().solve(-output);
                    input += adjust;

                    if (adjust.norm() < this->tolerance) {
                        out_position->x = input[0];
                        out_position->y = input[1];
                        return true;
                    }
                }
                return false;
            };
    };
}

