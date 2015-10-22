#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "drawable.hpp"
#include "simplexnoise.hpp"

class Terrain{
	public:
		Terrain(int _seed, int _octaveN);
		Drawable* getChunk(int x, int z);
		float getHeight(float x, float y);
	private:
		int seed;
		int octaves;
		float frequency;
		Simplex noise;
		Simplex amp;
		Simplex pers;
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

