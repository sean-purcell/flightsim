#include <cstdlib>

//default width and height of perlin noise generation
#define BLOCKWIDTH 10
#define BLOCKHEIGHT 10


sf::Color getRandomColor(){
	return sf::Color(std::rand()%255,std::rand()%255,std::rand()%255);
}

Triangle* splitTriangle(Triangle* tri, float hvarbound){
	//replaces a triangle with four other triangles. each of the three new edges the center is displaced by, at most, hvar up or down
	//works in-place on a linked list
	Drawable* next = tri->next;
	tri->next = 0;
	Quaternion midpoint = (tri->a + tri->b + tri->c)/3;
	Quaternion var = Quaternion(0,0,hvarbound*(std::rand()%2 - 1),0);
	midpoint = midpoint + var;
	Triangle* ret = new Triangle(tri->a, tri->b, midpoint, tri->color);
	ret->next = 	new Triangle(tri->a, midpoint, tri->c, tri->color);
	ret->next->next=new Triangle(midpoint, tri->b, tri->c, tri->color);
	ret->next->next->next = next;
	delete tri;
	return ret;
}

Drawable* generateTerrainBox(float x, float z, float width, float depth, float startY, int fractalIterations){
	//generates random terrain from (x to x+width) and (z to z+depth)
	sf::Color color = sf::Color::Green;
	Quaternion q = Quaternion(0,x,startY,z);
	Triangle* head = new Triangle(Quaternion(0,0,0,0)+q, Quaternion(0,width,0,0)+q, Quaternion(0,0,0,depth)+q, color);
	head->insert(new Triangle(Quaternion(0,width,0,depth)+q, Quaternion(0,width,0,0)+q, Quaternion(0,0,0,depth)+q, color));
	
	Triangle* iter;
	float var = 2.5;
	for (int i=0; i<fractalIterations; i++){
		head = splitTriangle(head,var);
		iter = head;
		while (iter->next->next->next!=NULL){
			iter->next->next->next = (Drawable*)splitTriangle((Triangle*)iter->next->next->next, var);
			iter = (Triangle*)iter->next->next->next;
		}
		var/=1.5;
	}
	
	
	return (Drawable*)head;
}

Drawable* iterateSplitTriangle(Triangle* tri, float variance, float varianceDecay, int fractalIterations){
	//generates random terrain from (x to x+width) and (z to z+depth)
	Triangle* iter;
	Triangle* head = tri;
	for (int i=0; i<fractalIterations; i++){
		head = splitTriangle(head,variance);
		iter = head;
		while (iter->next->next->next!=NULL){
			iter->next->next->next = (Drawable*)splitTriangle((Triangle*)iter->next->next->next, variance);
			iter = (Triangle*)iter->next->next->next;
		}
		variance*=varianceDecay;
	}
	
	
	return (Drawable*)head;
}

//implemented based on pseudocode on en.wikipedia.org/wiki/Perlin_noise

float lerp(float a, float b, float w){
	return (1.0-w)*a + w*b;
}

float dotGridGradient(float gradient[BLOCKWIDTH][BLOCKHEIGHT][2], int ix, int iy, float x, float y){
	float dx = x-(float)ix;
	float dy = y-(float)iy;
	
	return (dx*gradient[iy][ix][0] + dy*gradient[iy][ix][1]);
}

float perlin(float gradient[BLOCKWIDTH][BLOCKHEIGHT][2], float x, float y){
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
	std::cout<<n0<<" "<<n1<<" "<<sx<<" "<<ix1<<std::endl;
	return val;
}

void generatePerlin(float array[BLOCKWIDTH][BLOCKHEIGHT]){
	float gradient[BLOCKWIDTH][BLOCKHEIGHT][2];
	float theta;
	for (int i=0; i<BLOCKWIDTH; i++){
		for (int j=0; j<BLOCKHEIGHT; j++){
			theta = ((float)rand()/RAND_MAX)*(2*M_PI);
			gradient[i][j][0]=sin(theta);
			gradient[i][j][1]=cos(theta);
		}
	}
	
	for (int i=0; i<BLOCKWIDTH; i++){
		for (int j=0; j<BLOCKHEIGHT; j++){
			array[i][j]=perlin(gradient, i,j);
		}
	}
}

Drawable* perlinTerrain(int startX, int startY){
	Drawable* head = new Drawable();
	Drawable* iter = head;
	float array[BLOCKWIDTH][BLOCKHEIGHT];
	generatePerlin(array);
	
	for (int i=0; i<BLOCKWIDTH; i++){
		for (int j=0; j<BLOCKHEIGHT; j++){
			std::cout<<array[i][j]<<" ";
		}
		std::cout<<std::endl;
	}
	
	for (int i=0; i<BLOCKWIDTH-1; i++){
		for (int j=0; j<BLOCKHEIGHT-1; j++){
			iter->next = new Triangle(Quaternion(0, i, array[i][j], j), Quaternion(0, i+1, array[i+1][j], j), Quaternion(0, i, array[i][j+1], j+1), sf::Color::Green);
			iter->next->next = new Triangle(Quaternion(0, i+1, array[i+1][j+1], j+1), Quaternion(0, i+1, array[i+1][j], j), Quaternion(0, i, array[i][j+1], j+1), sf::Color::Green);
			iter = iter->next->next;
		}
	}
	
	Drawable* ret = head->next;
	head->next = 0;
	delete head;
	return ret;
}
