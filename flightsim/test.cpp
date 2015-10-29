#include "glmheaders.hpp"
#include <iostream>
#include <cmath>
using namespace std;

int main() {
	quat q = angleAxis((float)M_PI/2, vec3(0.f, 1.f, 0.f));

	vec3 v(1, 1, 1);

	vec3 a = q * v;

	cout << q.w << "," << q.x << "," << q.y << "," << q.z << endl;

	cout << a.x << "," << a.y << "," << a.z << endl;
}

