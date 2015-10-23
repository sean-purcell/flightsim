#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <utility>

#include "drawable.hpp"
#include "shapes.hpp"
#include "simplexnoise.hpp"

typedef std::pair<int, int> IntPair;
typedef std::map<IntPair, Drawable*> ChunkMap;

//default width and height of simplex noise generation
#define CHUNKCOUNT 32	//how many triangles wide makeup a CHUNK, it's nice if this is a power of 2
#define CHUNKWIDTH 128. //how wide/deep is the CHUNK in DISTANCE UNITS
#define CHUNKRATIO (CHUNKWIDTH/CHUNKCOUNT)
#define CHUNKDEGRADEDIST 200 // the distance increments it gets less accurate at

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

class TerrainChunk : public DrawableGroup {
	private:
		// the additional value at the end is to allow getHeight to
		// access outside the bounds and function properly
		float heightmap[CHUNKCOUNT+2][CHUNKCOUNT+2];
		Triangle triangles[CHUNKCOUNT*CHUNKCOUNT*2];
		std::pair<float, int> dists[CHUNKCOUNT * CHUNKCOUNT];
		Terrain &t;
		int x, z;

		float getHeight(float x, float y);

	public:
		TerrainChunk(int x, int z, Terrain &t);
		~TerrainChunk();

		void predraw(Quaternion camerapos, Quaternion camerarotation, Quaternion camerarotationinverse);
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

