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

void Sphere::predraw(vec3 camerapos, quat camerarotation){	//
	draw_pos = camerarotation * (pos - camerapos);
	
	distanceFromCamera = draw_pos.z;
	
	render_radius = ratio * (screenwidth / 2) * radius/distanceFromCamera;
	
	shape.setFillColor(color);

	if (draw_pos.z<0)
		shape.setRadius(0);
	else
		shape.setRadius(render_radius);
		
	shape.setPosition(getScreenPos(draw_pos)+sf::Vector2f(-render_radius,-render_radius));
	//std::cout<<"radius:"<<render_radius<<std::endl;
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

void Line::predraw(vec3 camerapos, quat camerarotation){	//
	drawStart = camerarotation * (start-camerapos);
	drawEnd = camerarotation * (end-camerapos);
	
	vertices[0].color = color;
	vertices[1].color = color;
	
	if ( (drawStart.z<0) and (drawEnd.z<0) ) {
		shouldDraw = false;
	}
	else {
		drawStart=clipLineToScreen(drawStart,drawEnd);
		drawEnd=clipLineToScreen(drawEnd,drawStart);
		shouldDraw = true;
	}

	distanceFromCamera = ((drawStart+drawEnd)/2.f).z;
	
	vertices[0].position = getScreenPos(drawStart);
	vertices[1].position = getScreenPos(drawEnd);

					
	//std::cout<<"radius:"<<render_radius<<std::endl;
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

void Triangle::predraw(vec3 camerapos, quat camerarotation){
#define eq(a, b) (fabs(a.x-b.x)<1e-3 && fabs(a.y-b.y)<1e-3)
	a_draw = camerarotation * (a-camerapos);
	b_draw = camerarotation * (b-camerapos);
	c_draw = camerarotation * (c-camerapos);

	if ( (a_draw.z<0) and (b_draw.z<0) and (c_draw.z<0) ) {
		shouldDraw = false;
		return;
	} else
		shouldDraw = true;
		
	distanceFromCamera = ((a_draw + b_draw + c_draw)/3.f).z;
	
	sf::Vector2f
		a0 = getScreenPos(clipLineToScreen(a_draw,c_draw)),
		a1 = getScreenPos(clipLineToScreen(a_draw,b_draw)),
		b0 = getScreenPos(clipLineToScreen(b_draw,a_draw)),
		b1 = getScreenPos(clipLineToScreen(b_draw,c_draw)),
		c0 = getScreenPos(clipLineToScreen(c_draw,b_draw)),
		c1 = getScreenPos(clipLineToScreen(c_draw,a_draw));

	if(eq(a0, a1) && eq(b0, b1) && eq(c0, c1)) {
		shape.setPointCount(3);
		shape.setPoint(0, a1);
		shape.setPoint(1, b1);
		shape.setPoint(2, c1);
	} else {
		shape.setPointCount(6);
		shape.setPoint(0, a1);
		shape.setPoint(1, b0);
		shape.setPoint(2, b1);
		shape.setPoint(3, c0);
		shape.setPoint(4, c1);
		shape.setPoint(5, a0);
	}
	
	int lmao = 255*(0.4+0.6*fabs(dot(GLOBAL_LIGHT_DIRECTION, getNormal())));
	shape.setFillColor(color*sf::Color(lmao, lmao, lmao));
#undef eq
}

void Triangle::draw(sf::RenderWindow &window){
	window.draw(shape);
	//std::cout<< a_draw.getScreenPos().x << "," << a_draw.getScreenPos().y << " " << b_draw.getScreenPos().x << "," << b_draw.getScreenPos().y << " " << c_draw.getScreenPos().x << "," << c_draw.getScreenPos().y<<std::endl;
}

