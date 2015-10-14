#include <iostream>
#include <cmath>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <cstdlib>

const int fov=90;
const float ratio=std::tan(fov/2.0);
const int screenwidth=1000;
const int screenheight=850;
const sf::Vector2i size(screenwidth, screenheight);

class Quaternion{
	public:
		float w, x, y, z;
		
		Quaternion(){};
		
		Quaternion(float nx, float ny, float nz){
			//if only three arguments given, assume real component is 0
			w = 0;
			x = nx;
			y = ny;
			z = nz;
		}
		
		Quaternion(float nw, float nx, float ny, float nz){
			w = nw;
			x = nx;
			y = ny;
			z = nz;
		}
		
		Quaternion operator+(Quaternion b){
			return Quaternion(w+b.w, x+b.x, y+b.y, z+b.z);
		}
		
		Quaternion operator-(Quaternion b){
			return Quaternion(w-b.w, x-b.x, y-b.y, z-b.z);
		}	
		
		Quaternion operator*(float b){
			return Quaternion(b*w, b*x, b*y, b*z);
		}
		
		Quaternion operator*(Quaternion b){
			/*
			 * float nw = w*b.w - x*bx - y*by - z*bz;
			 * float nx = w*bx + x*b.w + y*bz - z*by;
			 * float ny = w*by - x*bz + y*b.w + z*bx;
			 * float nz = w*bz + x*by - y*bx + z*b.w;
			 */
			return Quaternion(	w*b.w - x*b.x - y*b.y - z*b.z,
								w*b.x + x*b.w + y*b.z - z*b.y,
								w*b.y - x*b.z + y*b.w + z*b.x,
								w*b.z + x*b.y - y*b.x + z*b.w );
		}
		
		Quaternion operator/(float b){
			return Quaternion(w/b, x/b, y/b, z/b);
		}
		
		Quaternion inverse(){
			return Quaternion(w,-x,-y,-z)/get_magnitude();
		}
		
		float get_magnitude(){
			return std::pow( std::pow(w,2) + std::pow(x,2) + std::pow(y,2) + std::pow(z,2) , 0.5);
		}
		
		void set_magnitude(float newmag){
			float factor = newmag/get_magnitude();
			
			w = w * factor;
			x = x * factor;
			y = y * factor;
			z = z * factor;
			
		}
		
		void normalize(){
			set_magnitude(1.);
		}
		
		Quaternion normalized(){
			Quaternion Q(w, x, y, z);
			Q.normalize();
			return Q;
		}

		sf::Vector2f getScreenPos(){
			if (z==0)
				return sf::Vector2f(-1000,-1000);
			return sf::Vector2f(
				ratio*(screenwidth/2.0)*x/z + (screenwidth/2.0),
				ratio*(screenheight/2.0)*y/z + (screenheight/2.0) );
		}
};

Quaternion clipLineToScreen(Quaternion A, Quaternion B){
	// if A is z-positive, returns z
	// if A is z-negative, returns a point on the line connecting A and B which is just in front of the camera
	if (A.z>0) 
		return A;
	else {
		return A + (B-A)*((0.0001-A.z)/(B.z-A.z));
	}
}

class Drawable{
	public:
		float distanceFromCamera;
		bool shouldDraw;
		Drawable * next;
		Drawable * child;
		virtual void predraw(Quaternion camerapos, Quaternion camerarotation){};
		virtual void draw(sf::RenderWindow &window){};
		
		Drawable() {
			next = NULL;	//link to a separate item
			child = NULL;	//currently does nothing, IDEALLY this will allow us to "group" 
			distanceFromCamera=-1;
		}
		
		virtual ~Drawable() {
			delete next;	//good hygiene pointers/pointer hygiene
		}
		
		virtual void insert(Drawable* item){
			item->next = next;
			next = item;
		}
		
		virtual void update(float dt){
		}
		
};

class Sphere: public Drawable
{
	public:
		Quaternion pos;
		
		Sphere(Quaternion _pos, float _radius, sf::Color _color){
			pos = _pos;
			radius = _radius;
			color = _color;
			
			next = NULL;
			child = NULL;
		}
		
		void predraw(Quaternion camerapos, Quaternion camerarotation){	//
			draw_pos = camerarotation * ((pos-camerapos)* camerarotation.inverse());
			
			distanceFromCamera = draw_pos.get_magnitude();
			
			render_radius = radius/distanceFromCamera;
			
			shape.setFillColor(color);

			if (draw_pos.z<0)
				shape.setRadius(0);
			else
				shape.setRadius(render_radius);
				
			shape.setPosition(draw_pos.getScreenPos()+sf::Vector2f(-render_radius,-render_radius));
			//std::cout<<"radius:"<<render_radius<<std::endl;
		}
		
		void draw(sf::RenderWindow &window){
			window.draw(shape);
		}
		
	private:
		float radius;
		float render_radius;
		Quaternion draw_pos;
		sf::Color color;
		sf::CircleShape shape;
};

class Line: public Drawable
{
	public:
		Quaternion start, end;
		
		Line(Quaternion _start, Quaternion _end, sf::Color _color){
			start = _start;
			end = _end;
			color = _color;
			
			next = NULL;
			child = NULL;
		}
		
		void predraw(Quaternion camerapos, Quaternion camerarotation){	//
			drawStart = camerarotation * ((start-camerapos)* camerarotation.inverse());
			drawEnd = camerarotation * ((end-camerapos)* camerarotation.inverse());
		
		
			
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
	
			distanceFromCamera = ((drawStart+drawEnd)/2).get_magnitude();
			
			vertices[0].position = drawStart.getScreenPos();

			vertices[1].position = drawEnd.getScreenPos();

							
			//std::cout<<"radius:"<<render_radius<<std::endl;
		}
		
		void draw(sf::RenderWindow &window){
			window.draw(vertices, 2, sf::Lines);
		}
		
	private:
		sf::Color color;
		sf::Vertex vertices[2];
		Quaternion drawStart, drawEnd;
	
};

class Triangle: public Drawable
{
	public:
		Quaternion a, b, c;
		
		Triangle(Quaternion _a, Quaternion _b, Quaternion _c, sf::Color _color){
			a = _a;
			b = _b;
			c = _c;
			color = _color;
			
			next = NULL;
			child = NULL;
		}
		
		void predraw(Quaternion camerapos, Quaternion camerarotation){
			a_draw = camerarotation * ((a-camerapos)* camerarotation.inverse());
			b_draw = camerarotation * ((b-camerapos)* camerarotation.inverse());
			c_draw = camerarotation * ((c-camerapos)* camerarotation.inverse());

			if ( (a_draw.z<0) and (b_draw.z<0) and (c_draw.z<0) )
				shouldDraw = false;
			else
				shouldDraw = true;
				
			distanceFromCamera = ((a_draw + b_draw + c_draw)/3).get_magnitude();
			
			
			
			shape.setPointCount(6);
			shape.setPoint(0, clipLineToScreen(a_draw,b_draw).getScreenPos() );
			shape.setPoint(1, clipLineToScreen(b_draw,a_draw).getScreenPos() );
			shape.setPoint(2, clipLineToScreen(b_draw,c_draw).getScreenPos() );
			shape.setPoint(3, clipLineToScreen(c_draw,b_draw).getScreenPos() );
			shape.setPoint(4, clipLineToScreen(c_draw,a_draw).getScreenPos() );
			shape.setPoint(5, clipLineToScreen(a_draw,c_draw).getScreenPos() );
			
			shape.setFillColor(color);
			
		
		}
		
		void draw(sf::RenderWindow &window){
			window.draw(shape);
			//std::cout<< a_draw.getScreenPos().x << "," << a_draw.getScreenPos().y << " " << b_draw.getScreenPos().x << "," << b_draw.getScreenPos().y << " " << c_draw.getScreenPos().x << "," << c_draw.getScreenPos().y<<std::endl;
		}
		
	private:
		sf::ConvexShape shape;
		sf::Color color;
		Quaternion a_draw, b_draw, c_draw;
	
};

class Aircraft: public Drawable	//currently a hacked together proof of concept using a sphere.
	//IT DOESN'T INHERIT FROM SPHERE BECAUSE IT WILL EVENTUALLY NOT BE A SPHERE (hopefully)
{
	public:
		Quaternion pos, facing, velocity;
		
		Aircraft(){
			pos = Quaternion(0, 1, 1, 1);		//real component must be zero
			facing = Quaternion (0, 1, 0, 0);	//orientation
			velocity = Quaternion (0, 0, 0, 0);	//real component must be zero
			
			//bunch of sphere stuff that will be obsolete eventually
			radius = 1;
			color = sf::Color(255,255,255);
			
			next = NULL;
			child = NULL;
		}
		
		//have fun figuring out quaternions
		void pitch(float angle);	// <============
		void roll(float angle);		// <============ needs work
		void yaw(float angle);		// <============
		
		void predraw(Quaternion camerapos, Quaternion camerarotation){	//how does one draw a plane? by pretending it's a sphere. yoloooo
			draw_pos = camerarotation * ((pos-camerapos)* camerarotation.inverse());
			
			distanceFromCamera = draw_pos.get_magnitude();
			
			render_radius = radius/distanceFromCamera;
			
			shape.setFillColor(color);

			if (draw_pos.z<0)
				shape.setRadius(0);
			else
				shape.setRadius(render_radius);
				
			shape.setPosition(draw_pos.getScreenPos()+sf::Vector2f(-render_radius,-render_radius));
			//std::cout<<"radius:"<<render_radius<<std::endl;
		}
		
		void update(float dt){
			pos = pos + velocity*dt;
		}
		
		void draw(sf::RenderWindow &window){
			window.draw(shape);
		}
		
	private:
		float radius;
		float render_radius;
		Quaternion draw_pos;
		sf::Color color;
		sf::CircleShape shape;
};

//hacked together mergesort

Drawable* mergeSort(Drawable*);
Drawable* merge(Drawable*, Drawable*);
Drawable* split(Drawable*);


Drawable* mergeSort(Drawable *start) {
    Drawable *second;

    if (start == NULL)
        return NULL;
    else if (start->next == NULL)
        return start;
    else
    {
        second = split(start);
        return merge(mergeSort(start),mergeSort(second));
    }
}

Drawable* merge(Drawable* first, Drawable* second) {
	
    if (first == NULL) return second;
    else if (second == NULL) return first;
    else if (first->distanceFromCamera >= second->distanceFromCamera) //if I reverse the sign to >=, the behavior reverses
    {
        first->next = merge(first->next, second);
        return first;
    }
    else 
    {
        second->next = merge(first, second->next);
        return second;
    }
}

Drawable* split(Drawable* start) {
    Drawable* second;

    if (start == NULL) return NULL;
    else if (start->next == NULL) return NULL;
    else {
        second = start->next;
        start->next = second->next;
        second->next = split(second->next);
        return second;
    }
}
//END




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
	
	for (int i=0; i<100; i++){
		objects->insert(new Sphere(Quaternion(std::rand()%100,std::rand()%100,std::rand()%100),500, sf::Color(std::rand()%255,std::rand()%255,std::rand()%255) ));
	}
	
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
	objects->insert(new Triangle(Quaternion(0,-8,59), Quaternion(0,-23,58), Quaternion(0,-22,22), sf::Color(100,100,200)));
	
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
		aircraft->velocity = aircraft->velocity + camerarotation.inverse() * (Quaternion(0,0,0, 0.01 * (float)sf::Keyboard::isKeyPressed(sf::Keyboard::W)) * camerarotation);
		aircraft->velocity = aircraft->velocity + camerarotation.inverse() * (Quaternion(0,0,0, -0.01 * (float)sf::Keyboard::isKeyPressed(sf::Keyboard::S)) * camerarotation);
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))	//air brakes are totally a real thing
			aircraft->velocity = aircraft->velocity * 0.95;
		
		//dcamerapos.x=(float)sf::Keyboard::isKeyPressed(sf::Keyboard::D)-(float)sf::Keyboard::isKeyPressed(sf::Keyboard::A);
		//dcamerapos.y=(float)sf::Keyboard::isKeyPressed(sf::Keyboard::Z)-(float)sf::Keyboard::isKeyPressed(sf::Keyboard::X);
		//dcamerapos.z=(float)sf::Keyboard::isKeyPressed(sf::Keyboard::W)-(float)sf::Keyboard::isKeyPressed(sf::Keyboard::S);

		//dcamerapos=dcamerapos*0.1;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)){
			camerarotation=Quaternion(1, 0, 0 , 0.01).normalized()*camerarotation;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)){
			camerarotation=Quaternion(1, 0, 0, -0.01).normalized()*camerarotation;
		}
		
		window.clear(sf::Color(0, 0, 0));
		
		update_list(objects, 0.033);	//can add time clocking later, right now simulation time is set to frame speed
		
		predraw_list(objects, camerapos, camerarotation);
		
		draw_list(objects, window);
		
		window.display();
	}
	
	
	delete objects;
	std::cout<<"done deallocating"<<std::endl;
	
	
    return 0;
}

