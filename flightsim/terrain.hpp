#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "drawable.hpp"
#include "simplexnoise.hpp"

typedef std::pair<int, int> IntPair;
typedef std::map<IntPair, Drawable*> ChunkMap;

class Terrain{
	public:
		Terrain(int _seed, int _octaveN);
		Drawable* getChunk(IntPair);
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
		ChunkManager(Terrain _terrain);
		Drawable* getNewChunks(float x, float z, int chunksAround);	//deallocates old chunks, returns pointer to new chunks
	private:
		Terrain terrain;
		int loadedChunks;

		ChunkMap loaded;		
		void loadChunk(IntPair);
		void freeChunk(IntPair);
		int isLoaded(IntPair);
};
#endif

