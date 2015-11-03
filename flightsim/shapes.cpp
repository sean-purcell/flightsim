#include <cmath>

#include "shapes.hpp"
#include "drawable.hpp"
#include "windowinfo.hpp"

vec3 GLOBAL_LIGHT_DIRECTION = normalize(vec3(0, 1, 0.4));

Sphere::Sphere(vec3 _pos, float _radius, sf::Color _color):
	Drawable()
	{
	pos = _pos;
	radius = _radius;
	color = _color;
}

void Sphere::draw(sf::RenderWindow &window){
	window.draw(shape);
}

Line::Line(vec3 _start, vec3 _end, sf::Color _color):
	Drawable()
	{
	start = _start;
	end = _end;
	color = _color;
}

void Line::draw(sf::RenderWindow &window){
	window.draw(vertices, 2, sf::Lines);
}

Triangle::Triangle()
: Drawable()
{
	a = vec3(0, 0, 0);
	b = vec3(0, 0, 0);
	c = vec3(0, 0, 0);
	color = sf::Color(0, 0, 0);
}

Triangle::Triangle(vec3 _a, vec3 _b, vec3 _c, sf::Color _color): Drawable()
{
	a = _a;
	b = _b;
	c = _c;
	color = _color;
}

vec3 Triangle::getNormal(){
	return normalize(cross(b-a, c-a));
}

void Triangle::draw(sf::RenderWindow &window){
}

