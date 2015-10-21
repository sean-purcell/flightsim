#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include <iostream>
#include <string>

#include <SFML/Graphics.hpp>

class Quaternion{
	public:
		float w, x, y, z;
		
		Quaternion();
		
		Quaternion(float nx, float ny, float nz);
		
		Quaternion(float nw, float nx, float ny, float nz);

		// Creates a new quaternion with the given angle and axis
		Quaternion(float rot, Quaternion axis) ;
		
		Quaternion operator+(Quaternion b) const ;
		
		Quaternion operator-(Quaternion b) const ;	
		
		Quaternion operator*(float b) const ;
		
		Quaternion operator*(Quaternion b) const ;
		
		Quaternion operator/(float b) const ;
		
		Quaternion inverse() const ;
		
		float get_magnitude() const;
		
		void set_magnitude(float newmag);
		
		void normalize();
		
		Quaternion normalized() const ;

		float dot(Quaternion other);
		
		Quaternion cross(Quaternion other);

		sf::Vector2f getScreenPos();

		Quaternion transform(Quaternion p) ;

		Quaternion invtransform(Quaternion p) ;
};

Quaternion clipLineToScreen(Quaternion A, Quaternion B);

std::string toString(Quaternion q);

std::ostream & operator<<(std::ostream &out, Quaternion const &q);

bool isOnScreen(sf::Vector2f pos);

#endif

