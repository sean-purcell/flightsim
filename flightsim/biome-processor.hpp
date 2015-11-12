#ifndef BIOME_PROCESSOR_HPP
#define BIOME_PROCESSOR_HPP

#include "inc/lodepng.h"

#include <vector>

#include "openglheaders.hpp"

std::vector<unsigned char> loadBiomeImage(std::string filename);

vec3 getBiomeColor(float persistence, float amplitude, std::vector<unsigned char> &image);

#endif
