#ifndef BIOME_PROCESSOR_HPP
#define BIOME_PROCESSOR_HPP

#include "inc\lodepng.h"
#include <vector>
//#include "openglutil.hpp"


std::vector<unsigned char> loadBiomeImage(std::string& filename);

int getBiomeColor(float persistence, float amplitude, std::vector<unsigned char> image&);

#endif
