#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <cstdlib>

int fov=90;
float ratio=std::tan(fov/2.0);
int screenwidth=1024;
int screenheight=640;
sf::Vector2i size(screenwidth, screenheight);

#include "quaternion.hpp"

#include "drawable.hpp"
#include "shapes.hpp"

#include "aircraft.hpp"
#include "terrain.hpp"
#include "movingtestground.hpp"

extern float up, down, left, right;

void update_list(Drawable* &start, float dt){
	Drawable* iter = start;
	int length=0;

	while (iter!=NULL){
		iter->update(dt);
		iter = iter->next;
		length++;
	}
}
		
void predraw_list(Drawable* &start, Quaternion camerapos, Quaternion camerarotation, Quaternion camerarotationinverse){
	Drawable* iter = start;
	int length=0;

	while (iter!=NULL){
		iter->predraw(camerapos, camerarotation, camerarotationinverse);
		iter = iter->next;
		length++;
	}
	
	start = mergeSort(start);
}
		
void draw_list(Drawable* start, sf::RenderWindow &window){
	Drawable* iter = start;
	while (iter!=NULL){
		if(iter->shouldDraw)
			iter->draw(window);
		iter = iter->next;
	}
}

int main()
{
    sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	sf::RenderWindow window(sf::VideoMode(screenwidth, screenheight), "super basic flight simulator thing", sf::Style::Default, settings);
   	sf::Event        event;
   	
   	window.setMouseCursorVisible(false);

	std::cout<<"start"<<std::endl;

	std::cout<<"end"<<std::endl;

	Drawable* objects = new Drawable;	//linked list of drawable objects
	
	std::rand();//seed RNG
	
	Aircraft* aircraft = new Aircraft();
	objects->insert(aircraft);	
		
	Drawable *terrain = initializeTestTerrain();

	objects->insert(terrain);
		
	Quaternion camerapos(0, 0, 0, 0);
	Quaternion camerarotation(Quaternion(1, 0, 0, 0).normalized());
	Quaternion camerarotationinverse;
	Quaternion temprotation(0, 0, 0, 0);
	sf::Mouse::setPosition(sf::Vector2i(screenwidth/2, screenheight/2),window);

	sf::Clock gameClock;
	sf::Time frame = sf::seconds(1/30.);
	
	while (window.isOpen()) {
		sf::Time stT = gameClock.getElapsedTime();
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			
			if (event.type == sf::Event::KeyPressed){
			}
		}
		
		camerarotation.normalize();
		
		left  = (float) sf::Keyboard::isKeyPressed(sf::Keyboard::A);
		right = (float) sf::Keyboard::isKeyPressed(sf::Keyboard::D);
		up    = (float) sf::Keyboard::isKeyPressed(sf::Keyboard::W);
		down  = (float) sf::Keyboard::isKeyPressed(sf::Keyboard::S);

		//dcamerapos=dcamerapos*0.1;

		window.clear(sf::Color(0, 0, 0));
		
		update_list(objects, frame.asSeconds());	//can add time clocking later, right now simulation time is set to frame speed
		updateMovTerrain(aircraft->pos);
		
		camerapos = aircraft->pos - aircraft->facing.transform(
			Quaternion(0, 0, 5));
		camerarotation = aircraft->facing.inverse();
		camerarotationinverse = aircraft->facing;

		predraw_list(objects, camerapos, camerarotation, camerarotationinverse);
		
		draw_list(objects, window);
		
		window.display();

		sf::Time sleepT = gameClock.getElapsedTime() - stT;
		if(frame > sleepT) {
			sf::sleep(frame - sleepT);
		}
	}
	
	
	delete objects;
	std::cout<<"done deallocating"<<std::endl;
	
	
    return 0;
}

