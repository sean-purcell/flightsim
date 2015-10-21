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

class ChunkManager{
	public:
		std::pair<int, int> loaded[1000];
		
		ChunkManager(Terrain _terrain);
		Drawable* load(int, int);
		void free(int, int);
		int isLoaded(int, int);

	private:
		Terrain terrain;
		int loadedChunks;
};
#endif

