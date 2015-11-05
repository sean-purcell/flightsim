#ifndef ROTATION_HPP
#define ROTATION_HPP

#include <cmath>
#include "quaternion.hpp"

const int CONVENTION_ZYX = 0;
const float PI = acos(-1);

class Euler
{
    public:
        float a, b, c; // Extrinsically rotate in sequence a, b, c or intrinsically rotate in sequence c, b, a
        int convention;

        Euler();
        Euler(float na, float nb, float nc, int nconvention=CONVENTION_ZYX);
        static Euler fromRotation(Quaternion r, int convention=CONVENTION_ZYX);
        static Euler fromDirection(Quaternion a, int convention=CONVENTION_ZYX);
        static Euler fromDirection(float x, float y, float z, int convention=CONVENTION_ZYX);

        float heading();        // Only with ZYX convention
        float elevation();      // Only with ZYX convention
        float bank();           // Only with ZYX convention

        Quaternion toQuaternion();
        std::string toString();
};

float radians(float degrees);
float degrees(float radians);

#endif
