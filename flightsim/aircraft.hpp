#ifndef AIRCRAFT_HPP
#define AIRCRAFT_HPP

extern float up, down, left, right;

class Aircraft: public Drawable	//currently a hacked together proof of concept using a sphere.
	//IT DOESN'T INHERIT FROM SPHERE BECAUSE IT WILL EVENTUALLY NOT BE A SPHERE (hopefully)
{
	public:
		float g;
		float mass;
		float thrust;
		float aoi;
		float maxaileron;
		float minaileron;
		float maxelevator;
		float minelevator;
		float wingarea;
		float pitchmoi;
		float rollmoi;
		float aileronarea;
		float aileronradius;
		float elevatorarea;
		float elevatorradius;
		float rho;

		Quaternion pos, facing, velocity, omega;

		void init_params() {
			g = -9.8;
			mass = 23500;
			thrust = 62300 * 2;
			aoi = 10 * M_PI / 180;
			maxaileron = 45 * M_PI / 180;
			minaileron = -45 * M_PI / 180;
			maxelevator = 24 * M_PI / 180;
			minelevator = -10.5 * M_PI / 180;
			wingarea = 38;
			pitchmoi = 21935.779;
			rollmoi = 161820.94;
			aileronarea = 0.003;
			aileronradius = 5;
			elevatorarea = 3;
			elevatorradius = 8;
			rho = 1.225;
		}
		
		Aircraft(){
			init_params();

			pos = Quaternion(0, 1, 10, 1);		//real component must be zero
			facing = Quaternion (1, 0, 0, 0);	//orientation
			velocity = Quaternion (0, 0, 0, 500);	//real component must be zero
			omega = Quaternion(0, 0, 0, 0);
			
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
			netF = netF + fThrust();

			float rollA = tAileron() / rollmoi;
			float pitchA = tElevator() / pitchmoi;

			//Quaternion accel = netF * (1 / mass);
			Quaternion accel(0, 0, 0);

			this->pos = this->pos + this->velocity * dt + accel * (0.5f * dt * dt);
			this->velocity = this->velocity + accel * dt;

			std::cout << rollA << " " << pitchA << std::endl;
			Quaternion alpha(0, pitchA, 0, rollA);

			omega = omega + alpha * dt;

			if(omega.get_magnitude() > 1e-5) {
				std::cout << rollmoi << std::endl;
				Quaternion omegaVersor(omega.get_magnitude() * dt, facing.transform(omega));
				facing = omegaVersor * facing;
			}

			std::cout << "s:" << this->pos << ", v:" << this->velocity << ", a:" << accel << ", o:" << omega << std::endl;
		}

		Quaternion fGravity() {
			return Quaternion(0, 0, g * mass, 0);
		}

		Quaternion fWing() {
			Quaternion wingang(aoi, Quaternion(-1, 0, 0));
			Quaternion wn = facing.transform(wingang.transform(Quaternion(0, 1, 0)));
			Quaternion v = this->velocity * -1.0f;

			Quaternion lift = wn * rho * wingarea * fabs(v.dot(wn)) * (v.dot(wn));
			return lift;
		}

		Quaternion fThrust() {
			Quaternion fw = facing.transform(Quaternion(0, 0, 1));
			return fw * thrust;
		}

		float tAileron() {
			std::cout << left << " " << right << std::endl;
			float effect = left * maxaileron + right * minaileron;
			Quaternion ailangl(-aoi + effect, Quaternion(-1, 0, 0));
			Quaternion ailangr(aoi + effect, Quaternion( 1, 0, 0));

			Quaternion anl = facing.transform(ailangl.transform(Quaternion(0, 1, 0)));
			Quaternion anr = facing.transform(ailangr.transform(Quaternion(0, 1, 0)));

			std::cout << "anl: " << anl << ", anr:" << anr << std::endl;

			Quaternion v = this->velocity * -1.0f;
			Quaternion vl = v + facing.transform(Quaternion(0, -this->omega.z * aileronradius, 0));
			Quaternion vr = v + facing.transform(Quaternion(0, this->omega.z * aileronradius, 0));

			Quaternion liftl = anl * rho * aileronarea * fabs(vl.dot(anl)) * (vl.dot(anl));
			Quaternion liftr = anr * rho * aileronarea * fabs(vr.dot(anr)) * (vr.dot(anr));

			Quaternion lt = liftl.cross(facing.transform(Quaternion(-aileronradius, 0, 0)));
			Quaternion rt = liftr.cross(facing.transform(Quaternion(aileronradius, 0, 0)));
			// TODO: fix
			Quaternion torque = lt + rt;
			std::cout << "lt: " << lt << ", rt: " << rt << ", t: " << torque << std::endl;
			return torque.dot(facing.transform(Quaternion(0, 0, 1)));
		}

		float tElevator() {
			return 0;
		}
		
	private:
		float radius;
		float render_radius;
		Quaternion draw_pos;
		sf::Color color;
		sf::CircleShape shape;
};

#endif

