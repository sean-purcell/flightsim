#include <cmath>

#include "shapes.hpp"
#include "drawable.hpp"
#include "windowinfo.hpp"

Quaternion GLOBAL_LIGHT_DIRECTION = Quaternion(0, 0, 1, 1).normalized();

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
	a_draw = camerarotation * ((a-camerapos)* camerarotationinverse);
	b_draw = camerarotation * ((b-camerapos)* camerarotationinverse);
	c_draw = camerarotation * ((c-camerapos)* camerarotationinverse);

	if ( (a_draw.z<0) and (b_draw.z<0) and (c_draw.z<0) )
		shouldDraw = false;
	else
		shouldDraw = true;
		
	distanceFromCamera = ((a_draw + b_draw + c_draw)/3).z;
	
	
	
	shape.setPointCount(6);
	shape.setPoint(0, clipLineToScreen(a_draw,b_draw).getScreenPos() );
	shape.setPoint(1, clipLineToScreen(b_draw,a_draw).getScreenPos() );
	shape.setPoint(2, clipLineToScreen(b_draw,c_draw).getScreenPos() );
	shape.setPoint(3, clipLineToScreen(c_draw,b_draw).getScreenPos() );
	shape.setPoint(4, clipLineToScreen(c_draw,a_draw).getScreenPos() );
	shape.setPoint(5, clipLineToScreen(a_draw,c_draw).getScreenPos() );
	
	int lmao = 255*(0.4+0.6*fabs(GLOBAL_LIGHT_DIRECTION.dot(getNormal())));
	shape.setFillColor(color*sf::Color(lmao, lmao, lmao));
	

}

void Triangle::draw(sf::RenderWindow &window){
	window.draw(shape);
	//std::cout<< a_draw.getScreenPos().x << "," << a_draw.getScreenPos().y << " " << b_draw.getScreenPos().x << "," << b_draw.getScreenPos().y << " " << c_draw.getScreenPos().x << "," << c_draw.getScreenPos().y<<std::endl;
}

