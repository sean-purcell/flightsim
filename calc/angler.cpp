#include "angler.hpp"
#include <cassert>

/** Check that the rotation axes make sense.
 */
void check_asserts()
{
    assert(sqmag(STICK) == 1);
    assert(sqmag(PITCH_AXIS) == 1);
    assert(sqmag(ROLL_AXIS) == 1);
    
    assert(sqmag(PITCH_PERP) == 1);
    assert(sqmag(ROLL_PERP) == 1);
    
    assert(dot(STICK, PITCH_AXIS) == 0);
    assert(dot(PITCH_AXIS, ROLL_AXIS) == 0);
    assert(dot(STICK, ROLL_AXIS) == 0);
    assert(eq(cross(PITCH_AXIS, STICK), PITCH_PERP));
    assert(eq(cross(ROLL_AXIS, STICK), ROLL_PERP));
}

/** The g vector will be pointing up from the center of the Earth.
 */
double pitcher(int gx, int gy, int gz)
{
    double g[] = {gx, gy, gz};
    double g_stick = dot(g, STICK);
    double g_perp = dot(g, PITCH_PERP);
    double theta = -atan2(g_perp, g_stick); // the angle of g wrt controller is the negative of the angle of the controller wrt g
    if (fabs(theta) > max_theta)
        theta = theta < 0 ? -max_theta : max_theta;
    return theta * scale_pitch;
}

/** The g vector will be pointing up from the center of the Earth.
 */
double roller(int gx, int gy, int gz)
{
    double g[] = {gx, gy, gz};
    double g_stick = dot(g, STICK);
    double g_perp = dot(g, ROLL_PERP);
    double theta = -atan2(g_perp, g_stick); // the angle of g wrt controller is the negative of the angle of the controller wrt g
    if (fabs(theta) > max_theta)
        theta = theta < 0 ? -max_theta : max_theta;
    return theta * scale_roll;
}

double dot(double a[], double b[])
{
    double sum = 0;
    for (int i = 0; i < 3; ++i)
        sum += a[i] * b[i];
    return sum;
}

double* cross(double a[], double b[])
{
    double *c = (double*)malloc(sizeof(double) * 3);
    c[0] = a[1]*b[2] - a[2]*b[1];
	c[1] = -(a[0]*b[2] - a[2]*b[0]);
	c[2] = a[0]*b[1] - a[1]*b[0];
	return c;
}

double sqmag(double a[])
{
    double value = 0;
    for (int i = 0; i < 3; ++i)
        value += a[i] * a[i];
    return value;
}

double mag(double a[])
{
    return sqrt(sqmag(a));
}

void copy(double target[], double source[])
{
    for (int i = 0; i < 3; ++i)
        target[i] = source[i];
}

bool eq(double a[], double b[])
{
    for (int i = 0; i < 3; ++i)
        if (a[i] != b[i])
            return false;
    return true;
}

void set_stick(double x, double y, double z)
{
    STICK[0] = x;
    STICK[1] = y;
    STICK[2] = z;   
}

void set_pitch_axis(double x, double y, double z)
{
    PITCH_AXIS[0] = x;
    PITCH_AXIS[1] = y;
    PITCH_AXIS[2] = z;
}
void set_roll_axis(double x, double y, double z)
{
    ROLL_AXIS[0] = x;
    ROLL_AXIS[1] = y;
    ROLL_AXIS[2] = z;
}

/** Recalculates PITCH_PERP and ROLL_PERP.
 */
void recalc_perps()
{
    copy(PITCH_PERP, cross(PITCH_AXIS, STICK));
    copy(ROLL_PERP, cross(ROLL_AXIS, STICK));
}
