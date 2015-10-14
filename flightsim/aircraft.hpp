#ifndef AIRCRAFT_HPP
#define AIRCRAFT_HPP

class Aircraft: public Drawable	//currently a hacked together proof of concept using a sphere.
	//IT DOESN'T INHERIT FROM SPHERE BECAUSE IT WILL EVENTUALLY NOT BE A SPHERE (hopefully)
{
	public:
		float mass;
		float thrust;
		float aoi;
		float maxaileron;
		float wingarea;
		float aileronarea;
		float aileronradius;
		float elevatorarea;
		float elevatorradius;

		Quaternion pos, facing, velocity;
		
		Aircraft(){
			pos = Quaternion(0, 1, 1, 1);		//real component must be zero
			facing = Quaternion (0, 0, 1, 0);	//orientation
			velocity = Quaternion (0, 0, 0, 0);	//real component must be zero
			
			//bunch of sphere stuff that will be obsolete eventually
			radius = 50;
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

		void computeForces() {
			Quaternion netF(0, 0, 0, 0);
			Quaternion netT(0, 0, 0, 0);

#ifdef UNDEFINED
			netF += fGravity();
			netF += fWing();
			netF += fAileron();
			netF += fElevator();
			netF += fThrust();

			netT += tAileron();
			netT += tElevator();
#endif
		}

		Quaternion fGravity() {
			return Quaternion(0, 0, 9.8 * mass, 0);
		}

		Quaternion fWing() {
			Quaternion wingang(aoi, Quaternion(0, 0, 1));
			Quaternion wp = facing.transform(wingang.transform(Quaternion(1, 0, 0)));
			Quaternion wn = facing.transform(wingang.transform(Quaternion(0, 1, 0)));
			return Quaternion(0, 0, 0, 0);
		}

		Quaternion fThrust() {
			Quaternion fw = facing.transform(Quaternion(1, 0, 0));
			return fw * thrust;
		}
		
	private:
		float radius;
		float render_radius;
		Quaternion draw_pos;
		sf::Color color;
		sf::CircleShape shape;
};

#endif

