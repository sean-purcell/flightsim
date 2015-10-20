#ifndef SHAPES_HPP
#define SHAPES_HPP

#include <SFML/Graphics.hpp>

#include "drawable.hpp"
#include "quaternion.hpp"

class Sphere: public Drawable
{
	public:
		Quaternion pos;
		sf::Color color;
		
		Sphere(Quaternion _pos, float _radius, sf::Color _color);
		
		void predraw(Quaternion camerapos, Quaternion camerarotation, Quaternion camerarotationinverse);
		
		void draw(sf::RenderWindow &window);
		
	private:
		float radius;
		float render_radius;
		Quaternion draw_pos;
		sf::CircleShape shape;
};

class Line: public Drawable
{
	public:
		Quaternion start, end;
		sf::Color color;
		
		Line(Quaternion _start, Quaternion _end, sf::Color _color);

		void predraw(Quaternion camerapos, Quaternion camerarotation, Quaternion camerarotationinverse);
		
		void draw(sf::RenderWindow &window);
		
	private:
		sf::Vertex vertices[2];
		Quaternion drawStart, drawEnd;
	
};

class Triangle: public Drawable
{
	public:
		Quaternion a, b, c;
		sf::Color color;
		
		Triangle(Quaternion _a, Quaternion _b, Quaternion _c, sf::Color _color);

		Quaternion getNormal();
		
		void predraw(Quaternion camerapos, Quaternion camerarotation, Quaternion camerarotationinverse);
		
		void draw(sf::RenderWindow &window);
		
	private:
		sf::ConvexShape shape;
		Quaternion a_draw, b_draw, c_draw;
	
};

#endif

