#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "drawable.hpp"
#include "perlinnoise.hpp"

class Terrain{
	public:
		Terrain(int _seed, int _octaveN);
		Drawable* getChunk(int x, int z);
	private:
		int seed;
		PerlinNoise noise;
};

#endif

