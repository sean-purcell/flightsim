#include <cmath>
#include "rotation.hpp"

float radians(float degrees)
{
    return degrees / 180 * PI;
}

float degrees(float radians)
{
    return radians / PI * 180;
}
