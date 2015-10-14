#ifndef AIRCRAFT_HPP
#define AIRCRAFT_HPP

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

#endif

