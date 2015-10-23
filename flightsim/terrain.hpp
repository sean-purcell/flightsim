#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <utility>

#include "drawable.hpp"
#include "shapes.hpp"
#include "simplexnoise.hpp"

//default width and height of simplex noise generation
#define CHUNKCOUNT 32	//how many triangles wide makeup a CHUNK, it's nice if this is a power of 2
#define CHUNKWIDTH 128. //how wide/deep is the CHUNK in DISTANCE UNITS
#define CHUNKRATIO (CHUNKWIDTH/CHUNKCOUNT)
#define CHUNKDEGRADEDIST 200 // the distance increments it gets less accurate at

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

class TerrainChunk : public DrawableGroup {
	private:
		// the additional value at the end is to allow getHeight to
		// access outside the bounds and function properly
		float heightmap[CHUNKCOUNT+2][CHUNKCOUNT+2];
		Triangle triangles[CHUNKCOUNT*CHUNKCOUNT*2];
		std::pair<float, int> dists[CHUNKCOUNT * CHUNKCOUNT];
		int x, z;

		float getHeight(float x, float y);

	public:
		TerrainChunk(int x, int z, Terrain &t);
		~TerrainChunk();

		void predraw(Quaternion camerapos, Quaternion camerarotation, Quaternion camerarotationinverse);
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

