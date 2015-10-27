#include <glm/mat4x4.hpp>

#include "glutil.hpp"

#include "quaternion.hpp"

glm::mat4 cameraToViewM(Quaternion camerapos, Quaternion camerarotation) {
	return versToMat4(camerarotation) * transToMat4(camerapos * -1);
}

glm::mat4 transToMat4(Quaternion q) {
	glm::mat4 r;

	r[3][0] = q.x;
	r[3][1] = q.y;
	r[3][2] = q.z;

	return r;
}

glm::mat4 versToMat4(Quaternion q) {
	glm::mat4 r;
	float
		qxx = q.x * q.x,
		qyy = q.y * q.y,
		qzz = q.z * q.z,
		qxy = q.x * q.y,
		qxz = q.x * q.z,
		qyz = q.y * q.z,
		qwx = q.w * q.x,
		qwy = q.w * q.y,
		qwz = q.w * q.z;

	r[0][0] = 1 - 2 * (qyy +  qzz);
	r[0][1] = 2 * (qxy + qwz);
	r[0][2] = 2 * (qxz - qwy);

	r[1][0] = 2 * (qxy - qwz);
	r[1][1] = 1 - 2 * (qxx +  qzz);
	r[1][2] = 2 * (qyz + qwx);

	r[2][0] = 2 * (qxz + qwy);
	r[2][1] = 2 * (qyz - qwx);
	r[2][2] = 1 - 2 * (qxx +  qyy);

	return r;
}

