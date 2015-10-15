#ifndef AIRCRAFT_HPP
#define AIRCRAFT_HPP

class Aircraft: public Drawable	//currently a hacked together proof of concept using a sphere.
	//IT DOESN'T INHERIT FROM SPHERE BECAUSE IT WILL EVENTUALLY NOT BE A SPHERE (hopefully)
{
	public:
		float g = -9.8;
		float mass = 23500;
		float thrust = 62300 * 2;
		float aoi = 10 * M_PI / 180;
		float maxaileron;
		float wingarea = 38;
		float aileronarea;
		float aileronradius;
		float elevatorarea;
		float elevatorradius;
		float rho = 1.225;

		Quaternion pos, facing, velocity;
		
		Aircraft(){
			pos = Quaternion(0, 1, 100, 1);		//real component must be zero
			facing = Quaternion (1, 0, 0, 0);	//orientation
			velocity = Quaternion (0, 0, 0, 531);	//real component must be zero
			
			//bunch of sphere stuff that will be obsolete eventually
			radius = 0.05;
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
		
		void update(float dt){
			applyForces(dt);
		}
		
		void draw(sf::RenderWindow &window){
			window.draw(shape);
		}

		void applyForces(float dt) {
			Quaternion netF(0, 0, 0, 0);
			Quaternion netT(0, 0, 0, 0);

			netF = netF + fGravity();
			netF = netF + fWing();
			//netF += fAileron();
			//netF += fElevator();
			netF = netF + fThrust();

			//netT += tAileron();
			//netT += tElevator();

			Quaternion accel = netF * (1 / mass);

			this->pos = this->pos + this->velocity * dt + accel * (0.5f * dt * dt);
			this->velocity = this->velocity + accel * dt;

			std::cout << "s:" << this->pos << ", v:" << this->velocity << ", a:" << accel << ", f:" << (facing.transform(Quaternion(0, 0, 1))) << std::endl;
		}

		Quaternion fGravity() {
			return Quaternion(0, 0, g * mass, 0);
		}

		Quaternion fWing() {
			Quaternion wingang(aoi, Quaternion(-1, 0, 0));
			Quaternion wn = facing.transform(wingang.transform(Quaternion(0, 1, 0)));
			Quaternion v = this->velocity * -1.0f;

			Quaternion lift = wn * rho * wingarea * fabs(v.dot(wn)) * (v.dot(wn));
			std::cout << "wn: " << wn << ", lift: " << lift << std::endl;
			return lift;
		}

		Quaternion fThrust() {
			Quaternion fw = facing.transform(Quaternion(0, 0, 1));
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

