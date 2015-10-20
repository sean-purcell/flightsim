#include <cstdlib>
#include <cmath>

#include "terrain.hpp"
#include "drawable.hpp"
#include "shapes.hpp"

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

float lerp(float a, float b, float w){
	return (1.0-w)*a + w*b;
}

float dotGridGradient(float gradient[CHUNKCOUNT+2][CHUNKCOUNT+2][2], int ix, int iy, float x, float y){
	float dx = x-(float)ix;
	float dy = y-(float)iy;
	
	return (dx*gradient[iy][ix][0] + dy*gradient[iy][ix][1]);
}

float perlin(float gradient[CHUNKCOUNT+2][CHUNKCOUNT+2][2], float x, float y){
	int x0 = (x>0.0 ? (int)x : (int)x-1);
	int x1 = x0+1;
	int y0 = (y>0.0 ? (int)y : (int)y-1);
	int y1 = y0+1;

	float sx = x-(float)x0;
	float sy = y-(float)y0;

	float n0, n1, ix0, ix1, val;
	n0 = dotGridGradient(gradient, x0, y0, x, y);
	n1 = dotGridGradient(gradient, x1, y0, x, y);
	ix0 = lerp(n0, n1, sx);
	n0 = dotGridGradient(gradient, x0, y1, x, y);
	n1 = dotGridGradient(gradient, x1, y1, x, y);
	ix1 = lerp(n0, n1, sx);
	
	val = lerp(ix0, ix1, sy);
	return val;
}

void generatePerlin(int x, int z, float array[CHUNKCOUNT+1][CHUNKCOUNT+1]){
	float gradient[CHUNKCOUNT+2][CHUNKCOUNT+2][2];
	float theta;
	for (int i=0; i<CHUNKCOUNT+2; i++){
		for (int j=0; j<CHUNKCOUNT+2; j++){
			theta = srandTheta((x*(CHUNKCOUNT))+i,(z*(CHUNKCOUNT))+j);
			gradient[i][j][0]=sin(theta);
			gradient[i][j][1]=cos(theta);
		}
	}
	
	for (int i=0; i<CHUNKCOUNT+1; i++){
		for (int j=0; j<CHUNKCOUNT+1; j++){
			array[i][j]=heightDistort(perlin(gradient, i+0.5,j+0.5));
		}
	}
}

Drawable* perlinTerrain(int x, int z){
	//Returns a pointer to a linked list of Triangle objects that form a CHUNK of terrain that extend from (x*CHUNKWIDTH, z*CHUNKWIDTH) to ((x+1)*CHUNKWIDTH, (z+1)*CHUNKWIDTH)

	Drawable* head = new Drawable();
	Drawable* iter = head;
	float array[CHUNKCOUNT+1][CHUNKCOUNT+1];
	generatePerlin(x, z, array);
	
	/*for (int i=0; i<CHUNKCOUNT; i++){
		for (int j=0; j<CHUNKCOUNT; j++){
			array[i][j]=sqrt(fabs(array[i][j]));
		}
	}*/
	
	Quaternion start = Quaternion(0, z*CHUNKWIDTH, SEALEVEL, x*CHUNKWIDTH);
	
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
			
			if (srandBit(i,j)){
				iter->next = 		new Triangle(q1, q2, q3, TERRAINCOLOR);
				iter->next->next =  new Triangle(q4, q2, q3, TERRAINCOLOR);
			}
			else{
				iter->next = 		new Triangle(q1, q4, q3, TERRAINCOLOR);
				iter->next->next =  new Triangle(q1, q4, q2, TERRAINCOLOR);				
			}
			iter = iter->next->next;
		}
	}
	
	Drawable* ret = head->next;
	head->next = 0;
	delete head;
	return (Drawable*)(new DrawableGroup(ret));
}
