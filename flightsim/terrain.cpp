#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <functional>
#include <iostream>

#include "terrain.hpp"
#include "drawable.hpp"
#include "shapes.hpp"
#include "simplexnoise.hpp"

#define WATERCOLOR sf::Color(255,10,10)
#define TERRAINCOLOR sf::Color(50,200,50)
#define MOUNTAINCOLOR sf::Color(70,70,70)
#define SEALEVEL 0

float heightDistort(float height){
	//if you want to change how height is displayed, touch this instead of anything else please
	return 3*(height);
}

float srandTheta(int i, int j){
	srand(i*65537+j);
	return ((float)rand()/RAND_MAX)*2*M_PI;
}

int srandBit(int i, int j){
	srand(i*65537 + j);
	return (rand()>(RAND_MAX)/2);
}


sf::Color getColor(int x, int z, int q) {
	srand((x * 65537 + z) * (q ? 1 : -1));
	return sf::Color(rand() % 256, rand() % 256, rand() % 256);
}

//implemented based on pseudocode on en.wikipedia.org/wiki/Perlin_noise

Terrain::Terrain(int _seed, int _octaves) : seed(_seed),
	octaves(_octaves),
	frequency(0.5e-2),
	noise(frequency, 0.5, 25, _octaves, seed),
	amp(frequency * 1e-1, 0.3, 1, 3, 2*seed+1),
	pers(frequency * 1e-1, 0.3, 1, 3, 3*seed+7) {
	// frequency, persistence, amplitude, octaves, randomseed
}

float Terrain::getHeight(float x, float y){
	noise.set(frequency, 0.4+0.3*pers.getValue(x, y), 20*(1.1+amp.getValue(x, y)), octaves, seed);
	return noise.getValue(x, y);
}

Drawable* Terrain::getChunk(IntPair key){
	return new TerrainChunk(key.first, key.second, *this);
}

float interp(float a, float b, float r) {
	return a * (1-r) + b * r;
}

float TerrainChunk::getHeight(float x, float y) {
	return t.getHeight(x * CHUNKRATIO + this->x * CHUNKWIDTH,
		y * CHUNKRATIO + this->z * CHUNKWIDTH);
}

TerrainChunk::TerrainChunk(int _x, int _z, Terrain &_t)
	: DrawableGroup(&triangles[0]),
	x(_x),
	z(_z),
	t(_t)
{
}

TerrainChunk::~TerrainChunk() {
	for(int i = 0; i < CHUNKCOUNT*CHUNKCOUNT*2; i++) {
		triangles[i].next = NULL;
		triangles[i].child = NULL;
	}
	child = NULL;
}

float min(float a, float b) {
	return a < b ? a : b;
}

void TerrainChunk::predraw(vec3 camerapos, quat camerarotation) {
	shouldDraw = true;
	float dist;
	float _x = camerapos.x;
	float _y = camerapos.y;
	float _z = camerapos.z;

	float dx;
	float dz;

	if(_x >= x * CHUNKWIDTH && _x <= (x+1) * CHUNKWIDTH) {
		dx = 0;
	} else {
		dx = min(fabs(_x - x * CHUNKWIDTH),
			fabs(_x - (x + 1) * CHUNKWIDTH));
	}
	if(_z >= z * CHUNKWIDTH && _z <= (z + 1) * CHUNKWIDTH) {
		dz = 0;
	} else {
		dz = min(fabs(_z - z * CHUNKWIDTH),
			fabs(_z - (z + 1) * CHUNKWIDTH));
	}

	dist = sqrt(dx * dx + dz * dz + _y * _y);
	this->distanceFromCamera = dist;

	int TCOUNT = CHUNKCOUNT - (int) (dist / CHUNKDEGRADEDIST);
	//int TCOUNT = CHUNKCOUNT;
	TCOUNT = TCOUNT < 1 ? 1 : TCOUNT;

	float TRATIO = CHUNKWIDTH / TCOUNT;
	float HRATIO = CHUNKCOUNT / (float) TCOUNT;

	for(int i = 0; i < TCOUNT; i++) {
		for(int j = 0; j < TCOUNT; j++) {
			float dx = _x - (x * CHUNKWIDTH + (i + 0.5) * TRATIO);
			float dz = _z - (z * CHUNKWIDTH + (j + 0.5) * TRATIO);
			float dy = _y - getHeight((i + 0.5) * TRATIO, (j + 0.5) * TRATIO);
			dists[i * TCOUNT + j] = std::pair<float, int>(
				dx * dx + dz * dz + dy * dy,
				i * TCOUNT + j);
		}
	}


	sort(dists, dists + TCOUNT * TCOUNT, std::greater<std::pair<float,int> >());
	vec3 start = vec3(x * CHUNKWIDTH, SEALEVEL, z * CHUNKWIDTH);
	vec3 c1, c2, c3, c4;

	for(int k = 0; k < TCOUNT * TCOUNT; k++) {
		int idx = dists[k].second;
		int _x = idx / TCOUNT;
		int _z = idx % TCOUNT;

		float x0 = _x * TRATIO;
		float x1 = (_x + 1) * TRATIO;
		float z0 = _z * TRATIO;
		float z1 = (_z + 1) * TRATIO;

		float x0z0 = getHeight((_x+0) * HRATIO, (_z+0) * HRATIO);
		float x0z1 = getHeight((_x+0) * HRATIO, (_z+1) * HRATIO);
		float x1z0 = getHeight((_x+1) * HRATIO, (_z+0) * HRATIO);
		float x1z1 = getHeight((_x+1) * HRATIO, (_z+1) * HRATIO);

		c1 = vec3(x0, x0z0, z0);
		c2 = vec3(x1, x1z0, z0);
		c3 = vec3(x0, x0z1, z1);
		c4 = vec3(x1, x1z1, z1);

		c1 = c1 + start;
		c2 = c2 + start;
		c3 = c3 + start;
		c4 = c4 + start;

		if (srandBit(_x+x*TCOUNT,_z+z*TCOUNT)) {
			triangles[2*k  ].a = c1;
			triangles[2*k  ].b = c2;
			triangles[2*k  ].c = c3;
			triangles[2*k+1].a = c4;
			triangles[2*k+1].b = c2;
			triangles[2*k+1].c = c3;
		} else {
			triangles[2*k  ].a = c1;
			triangles[2*k  ].b = c4;
			triangles[2*k  ].c = c3;
			triangles[2*k+1].a = c1;
			triangles[2*k+1].b = c4;
			triangles[2*k+1].c = c2;
		}

		triangles[2*k  ].color = TERRAINCOLOR;
		triangles[2*k+1].color = TERRAINCOLOR;

		triangles[2 * k].next = &triangles[2 * k + 1];
		if(k != TCOUNT * TCOUNT - 1) {
			triangles[2 * k + 1].next = &triangles[2 * k + 2];
		} else {
			triangles[2 * k + 1].next = NULL;
		}

		triangles[2 * k].predraw(camerapos, camerarotation);
		triangles[2 * k+1].predraw(camerapos, camerarotation);
	}
}

ChunkManager::ChunkManager(Terrain _terrain):
	terrain(_terrain)
	{}

int len(Drawable *d);

Drawable* ChunkManager::getNewChunks(float x, float z, int chunksAround){
	//x,z are the coordinates of the player
	//distanceToLoad is the range around the player that you want to load chunks in
	int xchunk = floor(x/CHUNKWIDTH);
	int zchunk = floor(z/CHUNKWIDTH);
	
	IntPair key;
	
	Drawable* head = new Drawable;
	Drawable* iter = head;
	
	
	//remove any unneeded chunks
	ChunkMap::iterator tmp;
	for(ChunkMap::iterator iterator = loaded.begin(); iterator != loaded.end(); ) {
		if (abs(xchunk - iterator->first.first)>chunksAround || abs(zchunk - iterator->first.second)>chunksAround){ //if should be removed
			IntPair tmp = iterator->first;
			iterator++;
			freeChunk(tmp);
		} else {
			iterator++;
		}
	}
	
	//add any chunks that aren't in
	
	for (int i=xchunk-chunksAround; i<=xchunk+chunksAround; i++){
		for (int j=zchunk-chunksAround; j<=zchunk+chunksAround; j++){
			key = IntPair(i,j);
			if (!isLoaded(key)){
				loadChunk(key);
				iter->next = loaded[key];
				iter = iter->next;
			}
		}
	}
	
	Drawable* ret = head->next;
	head->next = 0;
	delete head;
	return static_cast<Drawable *>(ret);
}

void ChunkManager::loadChunk(IntPair key){
	if (!isLoaded(key)){
		loaded[key] = terrain.getChunk(key);
		loadedChunks++;
		std::cout << "adding chunk " << key.first << "," << key.second << std::endl;
	}
}

void ChunkManager::freeChunk(IntPair key){
	if (isLoaded(key)){
		loaded[key]->shouldRemove = true;
		loaded.erase(key);
		loadedChunks--;
		std::cout << "deling chunk " << key.first << "," << key.second << std::endl;
	}
}

int ChunkManager::isLoaded(IntPair key){
	return loaded.find(key)!=loaded.end();
}
