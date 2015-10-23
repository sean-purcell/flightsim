#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <functional>

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

Drawable* Terrain::getChunk(int x, int z){
	return new TerrainChunk(x, z, *this);
	//Returns a pointer to a linked list of Triangle objects that form a CHUNK of terrain that extend from (x*CHUNKWIDTH, z*CHUNKWIDTH) to ((x+1)*CHUNKWIDTH, (z+1)*CHUNKWIDTH)

	Drawable* head = new Drawable();
	Drawable* iter = head;

	float array[CHUNKCOUNT+1][CHUNKCOUNT+1];
	for (int i=0; i<CHUNKCOUNT+1; i++){
		for (int j=0; j<CHUNKCOUNT+1; j++){
			//array[i][j]=fmax(noise.GetHeight((float)(i+CHUNKCOUNT*x)*CHUNKWIDTH/CHUNKCOUNT, (float)(j+CHUNKCOUNT*z)*CHUNKWIDTH/CHUNKCOUNT),0.0);
			//array[i][j]=noise.GetHeight((float)(i+CHUNKCOUNT*x)*CHUNKWIDTH/CHUNKCOUNT, (float)(j+CHUNKCOUNT*z)*CHUNKWIDTH/CHUNKCOUNT);
			array[i][j] = getHeight((float)(i+CHUNKCOUNT*x)*CHUNKWIDTH/CHUNKCOUNT, (float)(j+CHUNKCOUNT*z)*CHUNKWIDTH/CHUNKCOUNT);
		}
	}

	/*for (int i=0; i<CHUNKCOUNT; i++){
		for (int j=0; j<CHUNKCOUNT; j++){
			array[i][j]=sqrt(fabs(array[i][j]));
		}
	}*/
	
	//sf::Color color(192 + std::rand() % 64, 192 + std::rand() % 64, 192 + std::rand() % 64);
	sf::Color color = TERRAINCOLOR;

	Quaternion start = Quaternion(0, x*CHUNKWIDTH, SEALEVEL, z*CHUNKWIDTH);

	Quaternion q1, q2, q3, q4;
	
	for (int i=0; i<CHUNKCOUNT; i++){
		for (int j=0; j<CHUNKCOUNT; j++){
			
			q1 = Quaternion(0, (float)i*CHUNKWIDTH/CHUNKCOUNT, array[i][j], (float)j*CHUNKWIDTH/CHUNKCOUNT);
			q2 = Quaternion(0, (float)(i+1)*CHUNKWIDTH/CHUNKCOUNT, array[i+1][j], (float)j*CHUNKWIDTH/CHUNKCOUNT);
			q3 = Quaternion(0, (float)i*CHUNKWIDTH/CHUNKCOUNT, array[i][j+1], (float)(j+1)*CHUNKWIDTH/CHUNKCOUNT);
			q4 = Quaternion(0, (float)(i+1)*CHUNKWIDTH/CHUNKCOUNT, array[i+1][j+1], (float)(j+1)*CHUNKWIDTH/CHUNKCOUNT);
			
			q1=q1+start;
			q2=q2+start;
			q3=q3+start;
			q4=q4+start;
			
			if (srandBit(i+x,j+z)){
				if ((q1.y != q2.y) || (q2.y != q3.y)){
					iter->next = new Triangle(q1, q2, q3, color);
					iter = iter->next;
				}
				if ((q4.y != q2.y) || (q2.y != q3.y)){
					iter->next = new Triangle(q4, q2, q3, color);
					iter = iter->next;
				}
			}
			else{
				if ((q1.y != q4.y) || (q4.y != q3.y)){
					iter->next = new Triangle(q1, q4, q3, color);
					iter = iter->next;
				}
				if ((q1.y != q2.y) || (q2.y != q4.y)){
					iter->next = new Triangle(q1, q4, q2, color);				
					iter = iter->next;
				}
			}
		}
	}
	
	Drawable* ret = head->next;
	head->next = 0;
	delete head;
	return (Drawable*)(new DrawableGroup(ret));
}

float interp(float a, float b, float r) {
	return a * (1-r) + b * r;
}

float TerrainChunk::getHeight(float x, float y) {
	int x0 = (int) x;
	int y0 = (int) y;
	int x1 = x + 1;
	int y1 = y + 1;

	float xfrac = x - x0;
	float yfrac = y - y0;

	float x0y0 = heightmap[x0][y0];
	float x1y0 = heightmap[x1][y0];
	float x0y1 = heightmap[x0][y1];
	float x1y1 = heightmap[x1][y1];

	return interp(interp(x0y0, x1y0, xfrac),
	              interp(x1y0, x1y1, xfrac),
	              yfrac);
}

#include <iostream>

TerrainChunk::TerrainChunk(int _x, int _z, Terrain &t)
	: DrawableGroup(&triangles[0]),
	x(_x),
	z(_z)
{
	for(int i = 0; i <= CHUNKCOUNT; i++) {
		for(int j = 0; j <= CHUNKCOUNT; j++) {
			heightmap[i][j] = t.getHeight(
				x * CHUNKWIDTH + i * CHUNKRATIO,
				z * CHUNKWIDTH + j * CHUNKRATIO);
		}
	}
	for(int i = 0; i <= CHUNKCOUNT; i++) {
		heightmap[i][CHUNKCOUNT+1] = heightmap[i][CHUNKCOUNT];
	}
	for(int i = 0; i <= CHUNKCOUNT; i++) {
		heightmap[CHUNKCOUNT+1][i] = heightmap[CHUNKCOUNT][i];
	}
	heightmap[CHUNKCOUNT+1][CHUNKCOUNT+1] = heightmap[CHUNKCOUNT][CHUNKCOUNT];

	for(int i = 0; i < CHUNKCOUNT; i++) {
		for(int j = 0; j < CHUNKCOUNT; j++) {
			std::cout << getHeight(i, j) << " ";
		}
		std::cout << std::endl;
	}
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

void TerrainChunk::predraw(Quaternion camerapos, Quaternion camerarotation, Quaternion camerarotationinverse) {
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

	int TCOUNT = CHUNKCOUNT - (int) (dist / CHUNKDEGRADEDIST);
	//int TCOUNT = CHUNKCOUNT;
	TCOUNT = TCOUNT < 1 ? 1 : TCOUNT;

	//std::cout << "x: " << x << ", z: " << z << ", TCOUNT: " << TCOUNT << std::endl;

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
	Quaternion start = Quaternion(x * CHUNKWIDTH, SEALEVEL, z * CHUNKWIDTH);
	Quaternion q1, q2, q3, q4;

	for(int k = 0; k < TCOUNT * TCOUNT; k++) {
		int idx = dists[k].second;
		int _x = idx / TCOUNT;
		int _z = idx % TCOUNT;
		//std::cout << k << ": " << _x << ", " << _z << std::endl;

		float x0 = _x * TRATIO;
		float x1 = (_x + 1) * TRATIO;
		float z0 = _z * TRATIO;
		float z1 = (_z + 1) * TRATIO;

		float x0z0 = getHeight((_x+0) * HRATIO, (_z+0) * HRATIO);
		float x0z1 = getHeight((_x+0) * HRATIO, (_z+1) * HRATIO);
		float x1z0 = getHeight((_x+1) * HRATIO, (_z+0) * HRATIO);
		float x1z1 = getHeight((_x+1) * HRATIO, (_z+1) * HRATIO);

		q1 = Quaternion(x0, x0z0, z0);
		q2 = Quaternion(x1, x1z0, z0);
		q3 = Quaternion(x0, x0z1, z1);
		q4 = Quaternion(x1, x1z1, z1);

		if (srandBit(_x+x*TCOUNT,_z+z*TCOUNT)) {
			triangles[2*k  ].a = q1;
			triangles[2*k  ].b = q2;
			triangles[2*k  ].c = q3;
			triangles[2*k+1].a = q4;
			triangles[2*k+1].b = q2;
			triangles[2*k+1].c = q3;
		} else {
			triangles[2*k  ].a = q1;
			triangles[2*k  ].b = q4;
			triangles[2*k  ].c = q3;
			triangles[2*k+1].a = q1;
			triangles[2*k+1].b = q4;
			triangles[2*k+1].c = q2;
		}

		triangles[2*k  ].color = TERRAINCOLOR;
		triangles[2*k+1].color = TERRAINCOLOR;

		triangles[2 * k].next = &triangles[2 * k + 1];
		if(k != TCOUNT * TCOUNT - 1) {
			triangles[2 * k + 1].next = &triangles[2 * k + 2];
		} else {
			triangles[2 * k + 1].next = NULL;
		}

		triangles[2 * k].predraw(camerapos, camerarotation, camerarotationinverse);
		triangles[2 * k+1].predraw(camerapos, camerarotation, camerarotationinverse);
	}
}

ChunkManager::ChunkManager(Terrain _terrain):
	terrain(_terrain)
	{}

//Drawable* ChunkManager::load(int, int);
//void ChunkManager::free(int, int);
//int ChunkManager::isLoaded(int, int);

