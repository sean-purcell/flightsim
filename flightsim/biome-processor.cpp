#include <iostream>
#include "biome-processor.hpp"
#include "openglutil.hpp"

std::vector<unsigned char> loadBiomeImage(const std::string filename){
	//reads biome image into img

	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filename);
	if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	return image;
}

vec3 getBiomeColor(float persistence, float amplitude, std::vector<unsigned char> image&){
	//assuming amplitude, persistence are both in [0,1]
	int pers = persistence*200;
	int amp = amplitude*200;
	return vec3(image[pers*200 + amp], image[pers*200 + amp + 1], image[pers*200 + amp + 2])//rgb? hopefully
}

int main(){

}
