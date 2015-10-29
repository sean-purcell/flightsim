#ifndef AIRCRAFT_HPP
#define AIRCRAFT_HPP

#include "glmheaders.hpp"

#include "drawable.hpp"
#include <SFML/Graphics.hpp>

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

		vec3 pos, velocity, omega;
		quat facing;

		void init_params();
		
		Aircraft();
		
		void predraw(vec3 camerapos, quat camerarotation);
		
		void update(float dt);
		
		void draw(sf::RenderWindow &window);

		void applyForces(float dt);

		vec3 fGravity();

		vec3 fWing();

		vec3 fThrust();

		vec3 fDrag();

		float tAileron();

		float tElevator();

		float tRudder();
		
	private:
		float radius;
		float render_radius;
		vec3 draw_pos;
		sf::Color color;
		sf::CircleShape shape;
};

#endif

