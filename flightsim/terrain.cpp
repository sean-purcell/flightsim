#include <cstdlib>
#include <cmath>

#include "terrain.hpp"
#include "drawable.hpp"
#include "shapes.hpp"
#include "perlinnoise.hpp"

//default width and height of perlin noise generation
#define CHUNKCOUNT 4	//how many triangles wide makeup a CHUNK, it's nice if this is a power of 2
#define CHUNKWIDTH 10. //how wide/deep is the CHUNK in DISTANCE UNITS

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

/*class Chunk{
	public: DrawableCollection

};*/

Terrain::Terrain(int _seed, int octaves){
	seed = _seed;
	// persistence, frequency, amplitude, octaves, randomseed
	noise = PerlinNoise(0.7, 0.1, 3, octaves, seed);
}

Drawable* Terrain::getChunk(int x, int z){
	//Returns a pointer to a linked list of Triangle objects that form a CHUNK of terrain that extend from (x*CHUNKWIDTH, z*CHUNKWIDTH) to ((x+1)*CHUNKWIDTH, (z+1)*CHUNKWIDTH)

	Drawable* head = new Drawable();
	Drawable* iter = head;

	float array[CHUNKCOUNT+1][CHUNKCOUNT+1];
	for (int i=0; i<CHUNKCOUNT+1; i++){
		for (int j=0; j<CHUNKCOUNT+1; j++){
			//array[i][j]=fmax(noise.GetHeight((float)(i+CHUNKCOUNT*x)*CHUNKWIDTH/CHUNKCOUNT, (float)(j+CHUNKCOUNT*z)*CHUNKWIDTH/CHUNKCOUNT),0.0);
			array[i][j]=noise.GetHeight((float)(i+CHUNKCOUNT*x)*CHUNKWIDTH/CHUNKCOUNT, (float)(j+CHUNKCOUNT*z)*CHUNKWIDTH/CHUNKCOUNT);
		}
	}

	/*for (int i=0; i<CHUNKCOUNT; i++){
		for (int j=0; j<CHUNKCOUNT; j++){
			array[i][j]=sqrt(fabs(array[i][j]));
		}
	}*/
	
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
					iter->next = 		new Triangle(q1, q2, q3, TERRAINCOLOR);
					iter = iter->next;
				}
				if ((q4.y != q2.y) || (q2.y != q3.y)){
					iter->next =  new Triangle(q4, q2, q3, TERRAINCOLOR);
					iter = iter->next;
				}
			}
			else{
				if ((q1.y != q4.y) || (q4.y != q3.y)){
					iter->next = 		new Triangle(q1, q4, q3, TERRAINCOLOR);
					iter = iter->next;
				}
				if ((q1.y != q2.y) || (q2.y != q4.y)){
					iter->next =  new Triangle(q1, q4, q2, TERRAINCOLOR);				
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

class ChunkManager{
	public:
		std::pair<int, int> loaded[1000];
		
		ChunkManager(Terrain _terrain) : terrain(_terrain) {}
		Drawable* load(int, int);
		void free(int, int);
		int isLoaded(int, int);

	private:
		Terrain terrain;
		int loadedChunks;
};



int ChunkManager::isLoaded(int x, int y){
	for (int i=0; i<loadedChunks; i++){
		
	}
}

