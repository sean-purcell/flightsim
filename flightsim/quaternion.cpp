#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>

#include "quaternion.hpp"
#include "windowinfo.hpp"

Quaternion::Quaternion(){};

Quaternion::Quaternion(float nx, float ny, float nz){
	//if only three arguments given, assume real component is 0
	w = 0;
	x = nx;
	y = ny;
	z = nz;
}

Quaternion::Quaternion(float nw, float nx, float ny, float nz){
	w = nw;
	x = nx;
	y = ny;
	z = nz;
}

// Creates a new quaternion with the given angle and axis
Quaternion::Quaternion(float rot, Quaternion axis) {
	axis.normalize();
	axis = axis * std::sin(rot/2);

	*this = axis;
	this->w = std::cos(rot/2);
}

Quaternion Quaternion::operator+(Quaternion b) const {
	return Quaternion(w+b.w, x+b.x, y+b.y, z+b.z);
}

Quaternion Quaternion::operator-(Quaternion b) const {
	return Quaternion(w-b.w, x-b.x, y-b.y, z-b.z);
}	

Quaternion Quaternion::operator*(float b) const {
	return Quaternion(b*w, b*x, b*y, b*z);
}

Quaternion Quaternion::operator*(Quaternion b) const {
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

Quaternion Quaternion::operator/(float b) const {
	return Quaternion(w/b, x/b, y/b, z/b);
}

Quaternion Quaternion::inverse() const {
	return Quaternion(w,-x,-y,-z)/get_magnitude();
}

float Quaternion::get_magnitude() const{
	return std::pow( std::pow(w,2) + std::pow(x,2) + std::pow(y,2) + std::pow(z,2) , 0.5);
}

void Quaternion::set_magnitude(float newmag){
	float factor = newmag/get_magnitude();
	
	w = w * factor;
	x = x * factor;
	y = y * factor;
	z = z * factor;
	
}

void Quaternion::normalize(){
	set_magnitude(1.);
}

Quaternion Quaternion::normalized() const {
	Quaternion Q(w, x, y, z);
	Q.normalize();
	return Q;
}

float Quaternion::dot(Quaternion other){
	return w*other.w + x*other.x + y*other.y + z*other.z;
}

Quaternion Quaternion::cross(Quaternion other){
	Quaternion tmp = (*this)*other;
	tmp.w=0;
	return tmp;
}

sf::Vector2f Quaternion::getScreenPos(){
	if (z==0)
		return sf::Vector2f(NAN, NAN);
	return sf::Vector2f(
		ratio*(screenwidth/2.0)*x/z + (screenwidth/2.0),
		-ratio*(screenwidth/2.0)*y/z + (screenheight/2.0) );
}

Quaternion Quaternion::transform(Quaternion p) {
	return *this * (p * this->inverse());
}

Quaternion Quaternion::invtransform(Quaternion p) {
	return this->inverse() * (p * *this);
}

Quaternion clipLineToScreen(Quaternion A, Quaternion B){
	// if A is z-positive, returns z
	// if A is z-negative, returns a point on the line connecting A and B which is just in front of the camera
	if (A.z>0) 
		return A;
	else if(B.z > 0) {
		return A + (B-A)*((0.0001-A.z)/(B.z-A.z));
	} else {
		return Quaternion(NAN, NAN, NAN);
	}
}

std::string toString(Quaternion q) {
	std::ostringstream os;
	os << std::setprecision(3) << std::fixed;
	os << "(" << q.w << "," << q.x << "," << q.y << "," << q.z << ")";
	return os.str();
}

std::ostream & operator<<(std::ostream &out, Quaternion const &q) {
	return out << (toString(q));
}

