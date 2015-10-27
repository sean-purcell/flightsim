#pragma once

#include <glm/mat4x4.hpp>

#include "quaternion.hpp"

glm::mat4 cameraToViewM(Quaternion camerapos, Quaternion camerarotation);
glm::mat4 transToMat4(Quaternion q);
glm::mat4 versToMat4(Quaternion q);

