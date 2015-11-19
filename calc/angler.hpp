#ifndef ANGLER_HPP
#define ANGLER_HPP

#include <cmath>

/** The derivation for the angle maths is in doc/derivations/angles.tex. */

/* Assuming that everything is in R3.
 * Using arrays of 3 doubles to represent vectors.
 */

extern double STICK[];          // direction of gravity when controller is in rest state
extern double PITCH_AXIS[];     // direction of pitch axis when controller is in rest state
extern double ROLL_AXIS[];      // direction of roll axis when controller is in rest state
extern double max_angle;        // magnitude of max rotation of controller; input clipped at this value if input is higher
extern double min_angle;        // magnitude of min rotation of controller; input clipped at this value if input is lower

double jpitch(short gx, short gy, short gz);
double jroll(short gx, short gy, short gz);

void check_asserts();
double dot(double a[], double b[]);
double* cross(double a[], double b[]);
double sqmag(double a[]);
double mag(double a[]);
bool eq(double a[], double b[]);

#endif
