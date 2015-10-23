#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include <SFML/Window.hpp>

#include "quaternion.hpp"

class Drawable{
	public:
		float distanceFromCamera;
		bool shouldDraw;
		bool shouldRemove;
		Drawable * next;
		Drawable * child;
		virtual void predraw(Quaternion camerapos, Quaternion camerarotation, Quaternion camerarotationinverse){};
		virtual void draw(sf::RenderWindow &window){};
		
		Drawable();
		
		virtual ~Drawable();
		
		virtual void insert(Drawable* item);
		
		virtual void update(float dt);
};

//hacked together mergesort

Drawable* mergeSort(Drawable*);
Drawable* merge(Drawable*, Drawable*);
Drawable* split(Drawable*);






class DrawableGroup: public Drawable
{
	public:
		DrawableGroup(Drawable* begin);
		
		~DrawableGroup();
		
		void insert(Drawable* item);
		virtual void predraw(Quaternion camerapos, Quaternion camerarotation, Quaternion camerarotationinverse);
		
		void draw(sf::RenderWindow &window);
};




#endif

