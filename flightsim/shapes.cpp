#include <cmath>

#include "shapes.hpp"
#include "drawable.hpp"
#include "windowinfo.hpp"

Quaternion GLOBAL_LIGHT_DIRECTION = Quaternion(0, 0, 1, 0.4).normalized();

Sphere::Sphere(Quaternion _pos, float _radius, sf::Color _color):
	Drawable()
	{
	pos = _pos;
	radius = _radius;
	color = _color;
}

void Sphere::predraw(Quaternion camerapos, Quaternion camerarotation, Quaternion camerarotationinverse){	//
	draw_pos = camerarotation * ((pos-camerapos)* camerarotationinverse);
	
	distanceFromCamera = draw_pos.z;
	
	render_radius = ratio * (screenwidth / 2) * radius/distanceFromCamera;
	
	shape.setFillColor(color);

	if (draw_pos.z<0)
		shape.setRadius(0);
	else
		shape.setRadius(render_radius);
		
	shape.setPosition(draw_pos.getScreenPos()+sf::Vector2f(-render_radius,-render_radius));
	//std::cout<<"radius:"<<render_radius<<std::endl;
}

void Sphere::draw(sf::RenderWindow &window){
	window.draw(shape);
}

Line::Line(Quaternion _start, Quaternion _end, sf::Color _color):
	Drawable()
	{
	start = _start;
	end = _end;
	color = _color;
}

void Line::predraw(Quaternion camerapos, Quaternion camerarotation, Quaternion camerarotationinverse){	//
	drawStart = camerarotation * ((start-camerapos)* camerarotationinverse);
	drawEnd = camerarotation * ((end-camerapos)* camerarotationinverse);


	
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

	distanceFromCamera = ((drawStart+drawEnd)/2).z;
	
	vertices[0].position = drawStart.getScreenPos();

	vertices[1].position = drawEnd.getScreenPos();

					
	//std::cout<<"radius:"<<render_radius<<std::endl;
}

void Line::draw(sf::RenderWindow &window){
	window.draw(vertices, 2, sf::Lines);
}

Triangle::Triangle()
: Drawable()
{
	a = Quaternion(0, 0, 0);
	b = Quaternion(0, 0, 0);
	c = Quaternion(0, 0, 0);
	color = sf::Color(0, 0, 0);
}

Triangle::Triangle(Quaternion _a, Quaternion _b, Quaternion _c, sf::Color _color): Drawable()
{
	a = _a;
	b = _b;
	c = _c;
	color = _color;
}

Quaternion Triangle::getNormal(){
	return ((b-a).cross(c-a)).normalized();
}

void Triangle::predraw(Quaternion camerapos, Quaternion camerarotation, Quaternion camerarotationinverse){
#define eq(a, b) (fabs(a.x-b.x)<1e-3 && fabs(a.y-b.y)<1e-3)
	a_draw = camerarotation * ((a-camerapos)* camerarotationinverse);
	b_draw = camerarotation * ((b-camerapos)* camerarotationinverse);
	c_draw = camerarotation * ((c-camerapos)* camerarotationinverse);

	if ( (a_draw.z<0) and (b_draw.z<0) and (c_draw.z<0) ) {
		shouldDraw = false;
		return;
	} else
		shouldDraw = true;
		
	distanceFromCamera = ((a_draw + b_draw + c_draw)/3).z;
	
	sf::Vector2f
		a0 = clipLineToScreen(a_draw,c_draw).getScreenPos(),
		a1 = clipLineToScreen(a_draw,b_draw).getScreenPos(),
		b0 = clipLineToScreen(b_draw,a_draw).getScreenPos(),
		b1 = clipLineToScreen(b_draw,c_draw).getScreenPos(),
		c0 = clipLineToScreen(c_draw,b_draw).getScreenPos(),
		c1 = clipLineToScreen(c_draw,a_draw).getScreenPos();

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
	
	int lmao = 255*(0.4+0.6*fabs(GLOBAL_LIGHT_DIRECTION.dot(getNormal())));
	shape.setFillColor(color*sf::Color(lmao, lmao, lmao));
#undef eq
}

void Triangle::draw(sf::RenderWindow &window){
	window.draw(shape);
	//std::cout<< a_draw.getScreenPos().x << "," << a_draw.getScreenPos().y << " " << b_draw.getScreenPos().x << "," << b_draw.getScreenPos().y << " " << c_draw.getScreenPos().x << "," << c_draw.getScreenPos().y<<std::endl;
}

