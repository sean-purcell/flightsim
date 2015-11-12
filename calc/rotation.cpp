#include <cassert>
#include <cmath>
#include <sstream>
#include <iomanip>
#include "rotation.hpp"
#include "quaternion.hpp"

Euler::Euler(){}

/** With ZYX convention:
 *      xE points north
 *      yE points east
 *      zE points down
 *  All parameters are in radians.
 */
Euler::Euler(float bank, float elevation, float heading)
{
    roll = bank;
    pitch = elevation;
    yaw = heading;
}

/** Returns an Tait-Bryan z-y-x angle representation of the given rotation quaternion.
 * Maths: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
 */
Euler Euler::fromRotation(Quaternion r)
{
    float q0 = r.w;
    float q1 = r.x;
    float q2 = r.y;
    float q3 = r.z;

    float a = atan2(2*(q0*q1 + q2*q3), 1 - 2*(q1*q1 + q2*q2));
    float b = asin(2*(q0*q2 - q3*q1));
    float c = atan2(2*(q0*q3 + q1*q2), 1 - 2*(q2*q2 + q3*q3));

    return Euler(a, b, c);
}

/** Return the quaternion representation of these Tait-Bryan z-y-x angles.
 * Maths: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
 */
Quaternion Euler::toQuaternion()
{
    Quaternion labyaw = Quaternion(cos(yaw/2), 0, 0, sin(yaw/2));
    Quaternion labpitch = Quaternion(cos(pitch/2), 0, sin(pitch/2), 0);
    Quaternion labroll = Quaternion(cos(roll/2), sin(roll/2), 0, 0);

    return labyaw * labpitch * labroll;
}

/** Returns a roll angle in the range (-180, 180] degrees.
 */
float Euler::rolld()
{
    return degrees(roll);
}

/** Returns a pitch angle in the range [-90, 90] degrees.
 */
float Euler::pitchd()
{
    return degrees(pitch);
}

/** Returns a yaw angle in the range [0, 360) degrees.
 */
float Euler::yawd()
{
    float h = degrees(yaw);
    if (h < 0) h += 360; // heading is positive
    return h;
}

std::string Euler::toString()
{
	std::ostringstream os;
	os << std::setprecision(3) << std::fixed;
	os << "(" << yawd() << "," << pitchd() << "," << rolld() << ")";
	return os.str();
}

float radians(float degrees)
{
    return degrees / 180 * PI;
}

float degrees(float radians)
{
    return radians / PI * 180;
}
