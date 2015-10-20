#ifndef AIRCRAFT_HPP
#define AIRCRAFT_HPP

#include "drawable.hpp"
#include "quaternion.hpp"
#include <SFML/Graphics.hpp>

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
		float yawmoi;
		float aileronarea;
		float aileronradius;
		float elevatorarea;
		float elevatorradius;
		float rudderarea;
		float rudderradius;
		float rho;
		float dragcoeff;
		float rudderdampcoeff;

		Quaternion pos, facing, velocity, omega;

		void init_params();
		
		Aircraft();
		
		void predraw(Quaternion camerapos, Quaternion camerarotation);
		
		void update(float dt);
		
		void draw(sf::RenderWindow &window);

		void applyForces(float dt);

		Quaternion fGravity();

		Quaternion fWing();

		Quaternion fThrust();

		Quaternion fDrag();

		float tAileron();

		float tElevator();

		float tRudder();
		
	private:
		float radius;
		float render_radius;
		Quaternion draw_pos;
		sf::Color color;
		sf::CircleShape shape;
};

#endif

