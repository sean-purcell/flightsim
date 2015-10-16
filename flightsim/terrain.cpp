#include <cstdlib>

//default width and height of perlin noise generation
#define BLOCKCOUNT_X 10	//how many triangles wide makeup a block 
#define BLOCKCOUNT_Z 5	//same but for z-values
#define BLOCKWIDTH 10. //how wide/deep is the block in distance units


#define TERRAINCOLOR sf::Color(50,200,50)
#define SEALEVEL 0

sf::Color getRandomColor(){
	return sf::Color(std::rand()%255,std::rand()%255,std::rand()%255);
}

float srandTheta(int i, int j){
	srand(i*65537+j);
	return ((float)rand()/RAND_MAX)*2*M_PI;
}

sf::Color getColor(int x, int z, int q) {
	srand((x * 65537 + z) * (q ? 1 : -1));
	return sf::Color(rand() % 256, rand() % 256, rand() % 256);
}

//implemented based on pseudocode on en.wikipedia.org/wiki/Perlin_noise

float lerp(float a, float b, float w){
	return (1.0-w)*a + w*b;
}

float dotGridGradient(float gradient[BLOCKCOUNT_X+2][BLOCKCOUNT_Z+2][2], int ix, int iy, float x, float y){
	float dx = x-(float)ix;
	float dy = y-(float)iy;
	
	return (dx*gradient[iy][ix][0] + dy*gradient[iy][ix][1]);
}

float perlin(float gradient[BLOCKCOUNT_X+2][BLOCKCOUNT_Z+2][2], float x, float y){
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

void generatePerlin(int x, int z, float array[BLOCKCOUNT_X+1][BLOCKCOUNT_Z+1]){
	float gradient[BLOCKCOUNT_X+2][BLOCKCOUNT_Z+2][2];
	float theta;
	for (int i=0; i<BLOCKCOUNT_X+2; i++){
		for (int j=0; j<BLOCKCOUNT_Z+2; j++){
			theta = srandTheta((x*(BLOCKCOUNT_X))+i,(z*(BLOCKCOUNT_Z))+j);
			gradient[i][j][0]=sin(theta);
			gradient[i][j][1]=cos(theta);
		}
	}
	
	std::cout<<"BEGIN BLOCK"<<std::endl;
	for (int i=0; i<BLOCKCOUNT_X+1; i++){
		for (int j=0; j<BLOCKCOUNT_Z+1; j++){
			array[i][j]=perlin(gradient, i+0.5,j+0.5);
			std::cout<<array[i][j]<<" ";
		}
		std::cout<<std::endl;
	}
	
	std::cout<<"END BLOCK"<<std::endl;
}

Drawable* perlinTerrain(int x, int z){
	//Returns a pointer to a linked list of Triangle objects that form a block of terrain that extend from (x*BLOCKWIDTH, z*BLOCKWIDTH) to ((x+1)*BLOCKWIDTH, (z+1)*BLOCKWIDTH)

	Drawable* head = new Drawable();
	Drawable* iter = head;
	float array[BLOCKCOUNT_X+1][BLOCKCOUNT_Z+1];
	generatePerlin(x, z, array);
	
	/*for (int i=0; i<BLOCKCOUNT_X; i++){
		for (int j=0; j<BLOCKCOUNT_Z; j++){
			array[i][j]=sqrt(fabs(array[i][j]));
		}
	}*/
	
	Quaternion start = Quaternion(0, z*BLOCKWIDTH, SEALEVEL, x*BLOCKWIDTH);
	
	Quaternion q1, q2, q3, q4;
	
	for (int i=0; i<BLOCKCOUNT_X; i++){
		for (int j=0; j<BLOCKCOUNT_Z; j++){
			
			q1 = Quaternion(0, (float)i*BLOCKWIDTH/BLOCKCOUNT_X, array[i][j], (float)j*BLOCKWIDTH/BLOCKCOUNT_Z);
			q2 = Quaternion(0, (float)(i+1)*BLOCKWIDTH/BLOCKCOUNT_X, array[i+1][j], (float)j*BLOCKWIDTH/BLOCKCOUNT_Z);
			q3 = Quaternion(0, (float)i*BLOCKWIDTH/BLOCKCOUNT_X, array[i][j+1], (float)(j+1)*BLOCKWIDTH/BLOCKCOUNT_Z);
			q4 = Quaternion(0, (float)(i+1)*BLOCKWIDTH/BLOCKCOUNT_X, array[i+1][j+1], (float)(j+1)*BLOCKWIDTH/BLOCKCOUNT_Z);
			
			q1=q1+start;
			q2=q2+start;
			q3=q3+start;
			q4=q4+start;
			
			if (rand()>(RAND_MAX/2)){
				iter->next = 		new Triangle(q1, q2, q3, getColor((x*BLOCKWIDTH)+i, (z*BLOCKWIDTH)+j, 0));
				iter->next->next =  new Triangle(q4, q2, q3, getColor((x*BLOCKWIDTH)+i, (z*BLOCKWIDTH)+j, 1));
			}
			else{
				iter->next = 		new Triangle(q1, q4, q3, getColor((x*BLOCKWIDTH)+i, (z*BLOCKWIDTH)+j, 0));
				iter->next->next =  new Triangle(q1, q4, q2, getColor((x*BLOCKWIDTH)+i, (z*BLOCKWIDTH)+j, 1));				
			}
			iter = iter->next->next;
		}
	}
	
	Drawable* ret = head->next;
	head->next = 0;
	delete head;
	return ret;
}
