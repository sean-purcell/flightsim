#ifndef ROTATION_HPP
#define ROTATION_HPP

#include <cmath>
#include "quaternion.hpp"

const float PI = acos(-1);

/** These angles are with ZYX convention.
 */
class Euler
{
    public:
        float roll, pitch, yaw; // Extrinsically rotate in sequence roll, pitch, yaw
                                // or intrinsically rotate in sequence yaw, pitch, roll
        Euler();
        Euler(float bank, float elevation, float heading);
        static Euler fromRotation(Quaternion r);

        float yawd();
        float pitchd();
        float rolld();

        Quaternion toQuaternion();
        std::string toString();
};

float radians(float degrees);
float degrees(float radians);

#endif
