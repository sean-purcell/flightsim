#include "angler.hpp"
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <algorithm>

/** The derivation for the angle maths is in doc/derivations/angles.tex. */

 double STICK[] = {-1, 0, 0};       // direction of gravity when controller is in rest state
 double PITCH_AXIS[] = {0, 1, 0};   // direction of pitch axis when controller is in rest state
 double ROLL_AXIS[] = {0, 0, -1};   // direction of roll axis when controller is in rest state
 double max_angle = acos(-1) / 2;   // clip angles above this value
 double min_angle = -acos(-1) / 2;  // clip angles below this value

/** Check that the rotation axes make sense.
 */
void check_asserts()
{
    assert(sqmag(STICK) == 1);
    assert(sqmag(PITCH_AXIS) == 1);
    assert(sqmag(ROLL_AXIS) == 1);

    assert(dot(STICK, PITCH_AXIS) == 0);
    assert(dot(PITCH_AXIS, ROLL_AXIS) == 0);
    assert(dot(STICK, ROLL_AXIS) == 0);
}

/** Return joystick pitch angle in radians.
 *
 * Theta calculated by projecting the given g vector onto the
 * pitch plane and doing trig maths from there.
 */
double jpitch(short gx, short gy, short gz)
{
    double g[] = {1.0*gx, 1.0*gy, 1.0*gz};
    double g_stick = dot(g, STICK);
    double g_roll = dot(g, ROLL_AXIS);
    double theta = atan2(g_roll, g_stick);
    theta = theta > max_angle ? max_angle : theta;
    theta = theta < min_angle ? min_angle : theta;
    return theta;
}

/** Return joystick roll angle in radians.
 *
 * Phi calculated by projecting the given g vector onto the
 * roll plane and doing trig maths from there.
 */
double jroll(short gx, short gy, short gz)
{
    double g[] = {1.0*gx, 1.0*gy, 1.0*gz};
    double g_stick = dot(g, STICK);
    double g_pitch = dot(g, PITCH_AXIS);
    double phi = atan2(-g_pitch, g_stick); // the angle of g wrt controller is the negative of the angle of the controller wrt g
    phi = phi > max_angle ? max_angle : phi;
    phi = phi < min_angle ? min_angle : phi;
    return phi;
}

/** Return dot product of given two vectors, assuming R3.
 */
double dot(double a[], double b[])
{
    double sum = 0;
    for (int i = 0; i < 3; ++i)
        sum += a[i] * b[i];
    return sum;
}

/** Return cross product of given two vectors, assuming R3.
 */
double* cross(double a[], double b[])
{
    double *c = (double*)malloc(sizeof(double) * 3);
    c[0] = a[1]*b[2] - a[2]*b[1];
	c[1] = -(a[0]*b[2] - a[2]*b[0]);
	c[2] = a[0]*b[1] - a[1]*b[0];
	return c;
}

/** Return square of norm of given vector, assuming R3.
 */
double sqmag(double a[])
{
    double value = 0;
    for (int i = 0; i < 3; ++i)
        value += a[i] * a[i];
    return value;
}

/** Return norm of given vector, assuming R3.
 */
double mag(double a[])
{
    return sqrt(sqmag(a));
}

/** Return true only if both given vectors are exactly equal, assuming R3.
 */
bool eq(double a[], double b[])
{
    for (int i = 0; i < 3; ++i)
        if (a[i] != b[i])
            return false;
    return true;
}
