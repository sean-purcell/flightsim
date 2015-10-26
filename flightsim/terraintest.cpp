#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <ctime>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <cstdlib>
 
int fov=90;
float ratio=std::tan(fov/2.0);
int screenwidth=800;
int screenheight=600;
sf::Vector2i size(screenwidth, screenheight);
 
#include "quaternion.hpp"
 
#include "drawable.hpp"
#include "shapes.hpp"
 
#include "aircraft.hpp"
#include "simplexnoise.hpp"
#include "terrain.hpp"

 int len(Drawable *d) {
	int r = 0;
	while(d != NULL) {
		d = d->next;
		r++;
	}
	return r;
 }
 
void update_list(Drawable** start, float dt){
	Drawable **iter = start;
	Drawable *tmp;
	int length=0;
 
	while (*iter!=NULL){
		Drawable *ptr = *iter;
		ptr->update(dt);
		if (ptr->shouldRemove){ //DELETE THAT SHIT
			std::cout << "removed object\n";
			*iter = ptr->next;
			ptr->next = NULL;
			delete ptr;
		} else {
			length++;
			iter = &(ptr->next);
		}
	}
	
	std::cout<<"length:"<<length<<std::endl;
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
 
	sf::RenderWindow window(sf::VideoMode(screenwidth, screenheight), "Objective Position Test w/ Quaternions", sf::Style::Default, settings);
	sf::Event		event;
	 
	window.setMouseCursorVisible(false);
 
	std::cout<<"start"<<std::endl;
 
	Drawable* objects = new Drawable;
	 
	std::srand(time(NULL));
	
	int seed = std::rand() % 65536;
	std::cout << "seed: " << seed << std::endl;
	Terrain terrain(seed, 10);
	ChunkManager chunkmanager(terrain);
	std::cout<<"end"<<std::endl;
	 
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
	Quaternion camerapos(0,0,0,0);
	Quaternion cameravel(0,0,0,0);
	Quaternion camerarotation (Quaternion(1,0,0,0).normalized());
	Quaternion camerarotationinverse;
	Quaternion temprotation(0,0,0,0);
	 
	int oldMouseX = screenwidth/2;
	int oldMouseY = screenheight/2;
	int dMouseX;
	int dMouseY;
	 
	sf::Mouse::setPosition(sf::Vector2i(screenwidth/2, screenheight/2),window);
	Drawable *newChunks = chunkmanager.getNewChunks(camerapos.x, camerapos.z, 1);
	if(newChunks != NULL) {
		objects->insert(newChunks);
	}
	 
	while (window.isOpen()) {
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			 
			if (event.type == sf::Event::KeyPressed){
			}
			 
			if (event.type == sf::Event::MouseMoved){
				dMouseX=event.mouseMove.x-oldMouseX;
				dMouseY=event.mouseMove.y-oldMouseY;
				 
				if ((dMouseX!=0) or (dMouseY!=0)){
					//std::cout<<dMouseX<<" "<<dMouseY<<std::endl;
					temprotation = Quaternion(1, -0.003*dMouseY, -0.003*dMouseX, 0).normalized();
					camerarotation = temprotation * camerarotation;
					 
					oldMouseX+=dMouseX;
					oldMouseY+=dMouseY;
				}
				 
 
			}
		}
		 
		camerarotation.normalize();
		camerarotationinverse = camerarotation.inverse();
		camerapos = camerapos + camerarotationinverse * (Quaternion(0,0,0, 10.11 * (float)sf::Keyboard::isKeyPressed(sf::Keyboard::W)) * camerarotation);
		camerapos = camerapos + camerarotationinverse * (Quaternion(0,0,0, -10.11 * (float)sf::Keyboard::isKeyPressed(sf::Keyboard::S)) * camerarotation);
		camerapos = camerapos + camerarotationinverse * (Quaternion(0, 10.11 * (float)sf::Keyboard::isKeyPressed(sf::Keyboard::D), 0, 0) * camerarotation);
		camerapos = camerapos + camerarotationinverse * (Quaternion(0, -10.11 * (float)sf::Keyboard::isKeyPressed(sf::Keyboard::A), 0, 0) * camerarotation);
		 
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
			cameravel = cameravel * 0.95;
		 
		//dcamerapos.x=(float)sf::Keyboard::isKeyPressed(sf::Keyboard::D)-(float)sf::Keyboard::isKeyPressed(sf::Keyboard::A);
		//dcamerapos.y=(float)sf::Keyboard::isKeyPressed(sf::Keyboard::Z)-(float)sf::Keyboard::isKeyPressed(sf::Keyboard::X);
		//dcamerapos.z=(float)sf::Keyboard::isKeyPressed(sf::Keyboard::W)-(float)sf::Keyboard::isKeyPressed(sf::Keyboard::S);
 
		//dcamerapos=dcamerapos*0.1;
 
		camerapos = camerapos + cameravel;
		 
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)){
			camerarotation=Quaternion(1, 0, 0 , 0.01).normalized()*camerarotation;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)){
			camerarotation=Quaternion(1, 0, 0, -0.01).normalized()*camerarotation;
		}
		 
		window.clear(sf::Color(100, 100, 240));
		
		Drawable *nchunks = chunkmanager.getNewChunks(camerapos.x, camerapos.z, 3);
		objects->insert(nchunks);

		update_list(&objects, 1);
		
		predraw_list(objects, camerapos, camerarotation, camerarotationinverse);
		 
		draw_list(objects, window);
		 
		window.display();
	}
	 
	 
	delete objects;
	std::cout<<"done deallocating"<<std::endl;
	 
	 
	return 0;
}
