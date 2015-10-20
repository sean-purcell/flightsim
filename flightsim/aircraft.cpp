#include <cmath>

#include "aircraft.hpp"
#include "quaternion.hpp"
#include "windowinfo.hpp"

void Aircraft::init_params() {
	g = -9.8;
	mass = 23500;
	thrust = 62300 * 2;
	aoi = 10 * M_PI / 180;
	maxaileron = 45 * M_PI / 180;
	minaileron = -45 * M_PI / 180;
	maxelevator = 24 * M_PI / 180;
	minelevator = -24 * M_PI / 180;
	wingarea = 38;
	pitchmoi = 21935.779;
	rollmoi = 161820.94;
	yawmoi = 178290.06;
	aileronarea = 0.03;
	aileronradius = 5;
	elevatorarea = 0.03;
	elevatorradius = 8;
	rudderarea = 0.3;
	rudderradius = 8;
	rho = 1.225;
	dragcoeff = 0.3;
	rudderdampcoeff = 100;
}

Aircraft::Aircraft(){
	init_params();

	pos = Quaternion(0, 1, 10000, 1);		//real component must be zero
	facing = Quaternion (1, 0, 0, 0);	//orientation
	velocity = Quaternion (0, 0, 0, 0);	//real component must be zero
	omega = Quaternion(0, 0, 0, 0);
	
	//bunch of sphere stuff that will be obsolete eventually
	radius = 0.05;
	color = sf::Color(255,255,255);

	next = NULL;
	child = NULL;
}

void Aircraft::predraw(Quaternion camerapos, Quaternion camerarotation){	//how does one draw a plane? by pretending it's a sphere. yoloooo
	draw_pos = camerarotation * ((pos-camerapos)* camerarotation.inverse());
	
	distanceFromCamera = draw_pos.z;
	
	render_radius = ratio * (screenwidth / 2) * radius/distanceFromCamera;
	
	shape.setFillColor(color);

	if (draw_pos.z<0)
		shape.setRadius(0);
	else
		shape.setRadius(render_radius);
		
	shape.setPosition(draw_pos.getScreenPos()+sf::Vector2f(-render_radius,-render_radius));
}

void Aircraft::update(float dt){
	applyForces(dt);
}

void Aircraft::draw(sf::RenderWindow &window){
	window.draw(shape);
}

void Aircraft::applyForces(float dt) {
	Quaternion netF(0, 0, 0, 0);

	netF = netF + fGravity();
	netF = netF + fWing();
	netF = netF + fThrust();
	netF = netF + fDrag();

	float rollA = tAileron() / rollmoi;
	float pitchA = tElevator() / pitchmoi;
	float yawA = tRudder() / yawmoi;

	Quaternion accel = netF * (1 / mass);

	this->pos = this->pos + this->velocity * dt + accel * (0.5f * dt * dt);
	this->velocity = this->velocity + accel * dt;

	Quaternion alpha(0, pitchA, yawA, rollA);

	omega = omega + alpha * dt;

	if(omega.get_magnitude() > 1e-5) {
		Quaternion omegaVersor(omega.get_magnitude() * dt, facing.transform(omega));
		facing = omegaVersor * facing;
	}

	std::cout << "s:" << this->pos << ", v:" << this->velocity << ", a:" << accel << ", f:" << (facing.transform(Quaternion(0, 0, 1))) << std::endl;
}

Quaternion Aircraft::fGravity() {
	return Quaternion(0, 0, g * mass, 0);
}

Quaternion Aircraft::fWing() {
	Quaternion wingang(aoi, Quaternion(-1, 0, 0));
	Quaternion wn = facing.transform(wingang.transform(Quaternion(0, 1, 0)));
	Quaternion v = this->velocity * -1.0f;

	Quaternion lift = wn * rho * wingarea * fabs(v.dot(wn)) * (v.dot(wn));
	return lift;
}

Quaternion Aircraft::fThrust() {
	Quaternion fw = facing.transform(Quaternion(0, 0, 1));
	return fw * thrust;
}

Quaternion Aircraft::fDrag() {
	Quaternion bw = facing.transform(Quaternion(0, 0, -1));
	return bw * (velocity.dot(bw)) * (velocity.dot(bw)) * rho * dragcoeff;
}

float Aircraft::tAileron() {
	float effect = left * maxaileron + right * minaileron;
	Quaternion ailangl(-aoi + effect, Quaternion(-1, 0, 0));
	Quaternion ailangr(aoi + effect, Quaternion( 1, 0, 0));

	Quaternion anl = facing.transform(ailangl.transform(Quaternion(0, 1, 0)));
	Quaternion anr = facing.transform(ailangr.transform(Quaternion(0, 1, 0)));

	Quaternion v = this->velocity * -1.0f;
	Quaternion vl = v + facing.transform(Quaternion(0, -this->omega.z * aileronradius, 0));
	Quaternion vr = v + facing.transform(Quaternion(0, this->omega.z * aileronradius, 0));

	Quaternion liftl = anl * rho * aileronarea * fabs(vl.dot(anl)) * (vl.dot(anl));
	Quaternion liftr = anr * rho * aileronarea * fabs(vr.dot(anr)) * (vr.dot(anr));

	Quaternion lt = liftl.cross(facing.transform(Quaternion(-aileronradius, 0, 0)));
	Quaternion rt = liftr.cross(facing.transform(Quaternion(aileronradius, 0, 0)));

	Quaternion torque = lt + rt;
	return torque.dot(facing.transform(Quaternion(0, 0, 1)));
}

float Aircraft::tElevator() {
	float effect = -up * minelevator + -down * maxelevator;
	Quaternion elangl(-effect, Quaternion(1, 0, 0));

	Quaternion en = facing.transform(elangl.transform(Quaternion(0, 1, 0)));

	Quaternion v = this->velocity * -1.0f
		+ facing.transform(Quaternion(0, omega.x * elevatorradius, 0));

	Quaternion lift = en * rho * elevatorarea * fabs(v.dot(en)) * (v.dot(en));

	Quaternion et = lift.cross(facing.transform(Quaternion(0, 0, elevatorradius)));

	return et.dot(facing.transform(Quaternion(1, 0, 0)));
}

float Aircraft::tRudder() {
	Quaternion rn = facing.transform(Quaternion(1, 0, 0));

	Quaternion v = this->velocity * -1.0f
		+ facing.transform(Quaternion(omega.y * rudderradius, 0, 0)) *
			rudderdampcoeff;

	Quaternion lift = rn * rho * rudderarea * fabs(v.dot(rn)) * (v.dot(rn));

	Quaternion rt = lift.cross(facing.transform(Quaternion(0, 0, rudderradius)));

	return rt.dot(facing.transform(Quaternion(0, 1, 0)));
}

