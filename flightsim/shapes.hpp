#ifndef SHAPES_HPP
#define SHAPES_HPP

#include <SFML/Graphics.hpp>

#include "drawable.hpp"
#include "glmheaders.hpp"

class Sphere: public Drawable
{
	public:
		vec3 pos;
		sf::Color color;
		
		Sphere(vec3 _pos, float _radius, sf::Color _color);
		
		void predraw(vec3 camerapos, quat camerarotation);
		
		void draw(sf::RenderWindow &window);
		
	private:
		float radius;
		float render_radius;
		vec3 draw_pos;
		sf::CircleShape shape;
};

class Line: public Drawable
{
	public:
		vec3 start, end;
		sf::Color color;
		
		Line(vec3 _start, vec3 _end, sf::Color _color);

		void predraw(vec3 camerapos, quat camerarotation);
		
		void draw(sf::RenderWindow &window);
		
	private:
		sf::Vertex vertices[2];
		vec3 drawStart, drawEnd;
	
};

class Triangle: public Drawable
{
	public:
		vec3 a, b, c;
		sf::Color color;
		
		Triangle();
		Triangle(vec3 _a, vec3 _b, vec3 _c, sf::Color _color);

		vec3 getNormal();
		
		void predraw(vec3 camerapos, quat camerarotation);
		
		void draw(sf::RenderWindow &window);
		
	private:
		sf::ConvexShape shape;
		vec3 a_draw, b_draw, c_draw;	
};

#endif

