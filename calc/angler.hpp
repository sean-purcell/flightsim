#ifndef ANGLER_HPP
#define ANGLER_HPP

#include <cmath>

/* Assumes that everything is in R3.
 */

double STICK[] = {0, 1, 0};   // direction of gravity when controller is in rest state
double PITCH_AXIS[] = {1, 0, 0};    // direction of pitch axis when controller is in rest state
double ROLL_AXIS[] = {0, 0, -1};    // direction of roll axis when controller is in rest state

double PITCH_PERP[] = {0, 0, 1};    // vector rotated 90 degrees counterclockwise about PITCH_AXIS from STICK direction
double ROLL_PERP[] = {1, 0, 0};     // vector rotated 90 degrees counterclockwise about ROLL_AXIS from STICK direction

double ZERO[] = {0, 0, 0};      // just the zero vector

double scale_pitch = 1 / 3.0;   // responsiveness of controller [rad/s per rad]
double scale_roll = 4 / 3.0;    // responsiveness of controller [rad/s per rad]
double max_theta = acos(-1) / 2; // magnitude of max rotation of controller; input clipped at this value if input is higher

double pitcher(int gx, int gy, int gz);
double roller(int gx, int gy, int gz);

double dot(double a[], double b[]);
double* cross(double a[], double b[]);
double sqmag(double a[]);
double mag(double a[]);
void copy(double target[], double source[]);
bool eq(double a[], double b[]);

void set_stick(double x, double y, double z);
void set_pitch_axis(double x, double y, double z);
void set_roll_axis(double x, double y, double z);
void recalc_perps();
void check_asserts();

#endif
