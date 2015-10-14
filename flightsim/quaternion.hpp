#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include <cmath>

#include <SFML/Graphics.hpp>

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


#endif

