#include <cstdlib>
#include <cmath>

#include "terrain.hpp"
#include "drawable.hpp"
#include "shapes.hpp"
#include "simplexnoise.hpp"

//default width and height of perlin noise generation
#define CHUNKCOUNT 16	//how many triangles wide makeup a CHUNK, it's nice if this is a power of 2
#define CHUNKWIDTH 40. //how wide/deep is the CHUNK in DISTANCE UNITS

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
	//Returns a pointer to a linked list of Triangle objects that form a CHUNK of terrain that extend from (x*CHUNKWIDTH, z*CHUNKWIDTH) to ((x+1)*CHUNKWIDTH, (z+1)*CHUNKWIDTH)
	int x = key.first;
	int z = key.second; 

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


ChunkManager::ChunkManager(Terrain _terrain):
	terrain(_terrain)
	{}

Drawable* ChunkManager::getNewChunks(float x, float z, int chunksAround){
	//x,z are the coordinates of the player
	//distanceToLoad is the range around the player that you want to load chunks in
	int xchunk = floor(x/CHUNKWIDTH);
	int zchunk = floor(z/CHUNKWIDTH);
	
	IntPair key;
	
	Drawable* head = new Drawable;
	Drawable* iter = head;
	
	std::cout<<"loaded:"<<loadedChunks<<std::endl;
	
	//remove any unneeded chunks
	ChunkMap::iterator tmp;
	for(ChunkMap::iterator iterator = loaded.begin(); iterator != loaded.end(); ) {
		if (abs(xchunk - iterator->first.first)>chunksAround || abs(zchunk - iterator->first.second)>chunksAround){ //if should be removed
			freeChunk((iterator++)->first);
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
	return (Drawable*)(new DrawableGroup(ret));
}

void ChunkManager::loadChunk(IntPair key){
	if (!isLoaded(key)){
		loaded[key] = terrain.getChunk(key);
		loadedChunks++;
	}
}

void ChunkManager::freeChunk(IntPair key){
	if (isLoaded(key)){
		loaded[key]->shouldRemove = true;
		loaded.erase(key);
		loadedChunks--;
	}
}

int ChunkManager::isLoaded(IntPair key){
	return loaded.find(key)!=loaded.end();
}
