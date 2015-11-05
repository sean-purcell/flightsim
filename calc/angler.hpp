#ifndef ANGLER_HPP
#define ANGLER_HPP

#include <cmath>

/* Assumes that everything is in R3.
 */

extern double STICK[];          // direction of gravity when controller is in rest state
extern double PITCH_AXIS[];     // direction of pitch axis when controller is in rest state
extern double ROLL_AXIS[];      // direction of roll axis when controller is in rest state

extern double PITCH_PERP[];     // vector rotated 90 degrees counterclockwise about PITCH_AXIS from STICK direction
extern double ROLL_PERP[];      // vector rotated 90 degrees counterclockwise about ROLL_AXIS from STICK direction

extern double scale_pitch;      // responsiveness of controller [rad/s per rad]
extern double scale_roll;       // responsiveness of controller [rad/s per rad]
extern double max_theta;        // magnitude of max rotation of controller; input clipped at this value if input is higher

double pitcher(int gx, int gy, int gz);
double roller(int gx, int gy, int gz);

double dot(double a[], double b[]);
double* cross(double a[], double b[]);
double sqmag(double a[]);
double mag(double a[]);
bool eq(double a[], double b[]);

void set_stick(double x, double y, double z);
void set_pitch_axis(double x, double y, double z);
void set_roll_axis(double x, double y, double z);
void recalc_perps();
void check_asserts();

#endif
