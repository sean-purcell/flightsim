#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <cstdlib>

const int fov=90;
const float ratio=std::tan(fov/2.0);
const int screenwidth=1600;
const int screenheight=1000;
const sf::Vector2i size(screenwidth, screenheight);

#include "quaternion.hpp"

Quaternion GLOBAL_LIGHT_DIRECTION = Quaternion(0, 0, 1, 1).normalized();

#include "drawable.hpp"
#include "shapes.hpp"

#include "aircraft.hpp"
#include "terrain.cpp"


void update_list(Drawable* &start, float dt){
	Drawable* iter = start;
	int length=0;

	while (iter!=NULL){
		iter->update(dt);
		iter = iter->next;
		length++;
	}
}
		
void predraw_list(Drawable* &start, Quaternion camerapos, Quaternion camerarotation){
	Drawable* iter = start;
	int length=0;

	while (iter!=NULL){
		iter->predraw(camerapos, camerarotation);
		iter = iter->next;
		length++;
	}
	
	start = mergeSort(start);
}
		
void draw_list(Drawable* start, sf::RenderWindow &window){
	Drawable* iter = start;
	while (iter!=NULL){
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
	
	/*for (int i=0; i<20; i++){
		for (int j=0; j<20; j++){
		objects->insert(new Sphere(Quaternion(0, i, 0,j),0.05, sf::Color(std::rand()%255,std::rand()%255,std::rand()%255) ));
		}
	}*/
	
	//objects->insert(generateTerrainBox(0, 0, 10, 10, 3, 2));
	//objects->insert(iterateSplitTriangle(new Triangle(Quaternion(0,-15,0,-15), Quaternion(0,20,0,0), Quaternion(0,0,0,20), sf::Color::Green) , 2.0, 0.5, 5));
	objects->insert(perlinTerrain(0,0));
	
	/*objects->insert(new Sphere(Quaternion(0,0,10),500,sf::Color(0,255,255)));
	objects->insert(new Sphere(Quaternion(0,10,0),500,sf::Color(0,255,255)));
	objects->insert(new Sphere(Quaternion(10,0,0),500,sf::Color(0,255,255)));
	objects->insert(new Sphere(Quaternion(10,10,10),500,sf::Color(0,255,255)));
	objects->insert(new Sphere(Quaternion(10,0,10),500,sf::Color(0,255,255)));
	objects->insert(new Sphere(Quaternion(10,10,0),500,sf::Color(0,255,255)));
	objects->insert(new Sphere(Quaternion(0,0,0),500,sf::Color(0,255,255)));
	objects->insert(new Sphere(Quaternion(0,10,10),500,sf::Color(0,255,255)));
	*/
	//objects.push_back(new Line(Quaternion(0,10,10), Quaternion(0,0,0) ,sf::Color(0,255,255)));
	//objects->insert(new Triangle(Quaternion(2,8,2), Quaternion(2,2,8), Quaternion(8,2,2), sf::Color(255,255,0)));
	//objects->insert(new Triangle(Quaternion(12,8,2), Quaternion(24,-23,58), Quaternion(18,22,22), sf::Color(255,100,100)));
	//objects->insert(new Triangle(Quaternion(0,-8,59), Quaternion(0,-23,58), Quaternion(0,-22,22), sf::Color(100,100,200)));
	
	Quaternion camerapos(0, 0, 0, 0);
	Quaternion camerarotation(Quaternion(1, 0, 0, 0).normalized());
	Quaternion temprotation(0, 0, 0, 0);
	sf::Mouse::setPosition(sf::Vector2i(screenwidth/2, screenheight/2),window);
	
	while (window.isOpen()) {
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			
			if (event.type == sf::Event::KeyPressed){
			}
		}
		
		camerarotation.normalize();
		
		//so uh we do input in an interesting way
		//this will probably be rewritten for ports or something but yeah
		//not only does it look like rubbish, it is in fact rubbish
		aircraft->velocity = aircraft->velocity + aircraft->facing.transform(
			Quaternion(0.0, 0,0, 0.01 *
			(float)sf::Keyboard::isKeyPressed(sf::Keyboard::E)));
		aircraft->velocity = aircraft->velocity + aircraft->facing.transform(
			Quaternion(0.0, 0,0, -0.01 *
			(float)sf::Keyboard::isKeyPressed(sf::Keyboard::Q)));
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))	//air brakes are totally a real thing
			aircraft->velocity = aircraft->velocity * 0.95;
		
		float pitchang = (0.01) * (float)sf::Keyboard::isKeyPressed(sf::Keyboard::W)
			+ (-0.01) * (float)sf::Keyboard::isKeyPressed(sf::Keyboard::S);
		Quaternion pitch(pitchang, aircraft->facing.transform(
			Quaternion(1, 0, 0)));
		//aircraft->velocity = pitch.transform(aircraft->velocity);
		aircraft->facing = pitch * aircraft->facing;

		float rollang = (0.01) * (float)sf::Keyboard::isKeyPressed(sf::Keyboard::A)
			+ (-0.01) * (float)sf::Keyboard::isKeyPressed(sf::Keyboard::D);
		Quaternion roll(rollang, aircraft->facing.transform(Quaternion(0, 0, 1)));
		//aircraft->velocity = roll.transform(aircraft->velocity);
		aircraft->facing = roll * aircraft->facing;
		//dcamerapos.x=(float)sf::Keyboard::isKeyPressed(sf::Keyboard::D)-(float)sf::Keyboard::isKeyPressed(sf::Keyboard::A);
		//dcamerapos.y=(float)sf::Keyboard::isKeyPressed(sf::Keyboard::Z)-(float)sf::Keyboard::isKeyPressed(sf::Keyboard::X);
		//dcamerapos.z=(float)sf::Keyboard::isKeyPressed(sf::Keyboard::W)-(float)sf::Keyboard::isKeyPressed(sf::Keyboard::S);

		//dcamerapos=dcamerapos*0.1;

		window.clear(sf::Color(0, 0, 0));
		
		update_list(objects, 0.033);	//can add time clocking later, right now simulation time is set to frame speed
		
		camerapos = aircraft->pos - aircraft->facing.transform(
			Quaternion(0, 0, 5));
		camerarotation = aircraft->facing.inverse();

		predraw_list(objects, camerapos, camerarotation);
		
		draw_list(objects, window);
		
		window.display();
	}
	
	
	delete objects;
	std::cout<<"done deallocating"<<std::endl;
	
	
    return 0;
}

