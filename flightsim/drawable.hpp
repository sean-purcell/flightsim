#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include <SFML/Graphics/RenderWindow.hpp>

#include "glmheaders.hpp"

class Drawable{
	public:
		float distanceFromCamera;
		bool shouldDraw;
		bool shouldRemove;
		Drawable * next;
		Drawable * child;
		virtual void predraw(vec3 camerapos, quat camerarotation);
		virtual void draw(sf::RenderWindow &window);

		Drawable();
		
		virtual ~Drawable();
		
		void insert(Drawable* item);
		
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
		
		virtual ~DrawableGroup();
		
		void insertInto(Drawable* item);
		virtual void predraw(vec3 camerapos, quat camerarotation);
		
		void draw(sf::RenderWindow &window);
};

sf::Vector2f getScreenPos(vec3 q);

vec3 clipLineToScreen(vec3 A, vec3 B);

#endif

