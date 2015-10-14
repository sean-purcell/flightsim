//very bad practice, only put in so it compiles
#include <cmath>

const int fov=90;
const float ratio=std::tan(fov/2.0);
const int screenwidth=1000;
const int screenheight=850;

#include <cmath>
#include "quaternion.hpp"
#include "drawable.hpp"
#include "shapes.hpp"

sf::Color getRandomColor(){
	return sf::Color(std::rand()%255,std::rand()%255,std::rand()%255);
}

Triangle* splitTriangle(Triangle* tri, float hvarbound){
	//replaces a triangle with three other triangles. the center is displaced by, at most, hvar up or down
	//works in-place on a linked list
	Drawable* next = tri->next;
	tri->next = 0;
	Quaternion midpoint = (tri->a + tri->b + tri->c)/3;
	Quaternion var = Quaternion(0,0,0,hvarbound*(std::rand()%2 - 1));
	midpoint = midpoint + var;
	Triangle* ret = new Triangle(tri->a, tri->b, midpoint, tri->color);
	ret->next = 	new Triangle(tri->a, tri->b + var,	tri->c, tri->color);
	ret->next->next=new Triangle(midpoint, tri->b, tri->c, tri->color);
	ret->next->next->next=next;
	delete tri;
	return ret;
}

Drawable* generateTerrainBox(float x, float z, float width, float depth, int fractalIterations){
	//generates random terrain from (x to x+width) and (z to z+depth)
	Quaternion q = Quaternion(0,x,0,z);
	Triangle* head = new Triangle(Quaternion(0,0,0,0)+q, Quaternion(0,width,0,0)+q, Quaternion(0,0,0,depth)+q, getRandomColor());
	head->next = new Triangle(Quaternion(0,width,0,depth)+q, Quaternion(0,width,0,0)+q, Quaternion(0,0,0,depth)+q, getRandomColor());
	
	Triangle* iter;
	float var = 3;
	for (int i=0; i<fractalIterations; i++){
		iter = head;
		iter = splitTriangle(iter, var);
		iter = (Triangle*)iter->next;
		var/=2;
	}
	
	
	return (Drawable*)head;
}
