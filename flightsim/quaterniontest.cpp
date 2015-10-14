#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

const int fov=90;
const float ratio=std::tan(fov/2.0);
const int screenwidth=1000;
const int screenheight=850;
const sf::Vector2i size(screenwidth, screenheight);

#include "quaternion.hpp"

std::string toString(Quaternion q) {
	std::ostringstream os;
	os << "(" << q.w << "," << q.x << "," << q.y << "," << q.z << ")";
	return os.str();
}

int main() {
	std::cout << toString(Quaternion(0, 0, 0, 0)) << std::endl;
	std::cout << toString(Quaternion(1, 0, 0, 0)) << std::endl;

	Quaternion facing(0.785, Quaternion(1, 0, 0));
	std::cout << toString(facing) << std::endl;
	std::cout << toString(facing.invtransform(Quaternion(0, 1, 0, 0))) << std::endl;
	std::cout << toString(facing.invtransform(Quaternion(0, 0, 1, 0))) << std::endl;
	std::cout << toString(facing.invtransform(Quaternion(0, 0, 0, 1))) << std::endl;
}

