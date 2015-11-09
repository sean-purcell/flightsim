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

vec3 getBiomeColor(float persistence, float amplitude, std::vector<unsigned char> &image){
	//assuming amplitude, persistence are both in [0,1]
	int pers = persistence*100 + 100;
	int amp = amplitude*100 + 100;
	vec3 col = vec3(image[(pers*200 + amp) * 4 + 0], image[(pers*200 + amp) * 4 + 1], image[(pers*200 + amp) * 4 + 2]) / 255.f;//rgb? hopefully
	return col;
}

