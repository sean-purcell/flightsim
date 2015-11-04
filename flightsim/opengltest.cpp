#include "openglheaders.hpp"

#include <ctime>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>
#include <iomanip>

#include "openglshaders.hpp"
#include "openglutil.hpp"
#include "terrain.hpp"

GLint uniTrans;
GLint uniView;
GLuint vao;

vec3 camerapos(0, 0, 3);
quat camerarotation(1,0,0,0);

bool left, right, up, down;
int mx, my;

Terrain terrain(0, 0);
ChunkManager chunkmanager(terrain);
TerrainChunk *chunks;

vec3 GLOBAL_LIGHT_DIRECTION = normalize(vec3(0, 1, 0.4));

void drawTerrain() {
	TerrainChunk *iter = chunks;
	while(iter) {
		iter->draw();
		iter = iter->next;
	}
}

void updateTerrain() {
	TerrainChunk *nchunks = chunkmanager.getNewChunks(camerapos.x, camerapos.z, 30);
	if(!chunks) {
		chunks = nchunks;
	} else {
		TerrainChunk *iter = chunks;
		while(iter->next) iter = iter->next;
		iter->next = nchunks;
	}

	int len = 0;
	TerrainChunk **cur = &chunks;
	while(*cur) {
		if((*cur)->shouldRemove) {
			*cur = (*cur)->next;
		} else {
			cur = &(*cur)->next;
			len++;
		}
	}
	std::cout << "number of chunks: " << len << std::endl;
}

void initTerrain() {
	std::srand(time(NULL));
	
	int seed = std::rand() % 65536;
	std::cout << "seed: " << seed << std::endl;
	terrain = Terrain(seed, 10);
	chunkmanager = ChunkManager(terrain);

	chunks = NULL;
	updateTerrain();
}

void init() // Called before main loop to set up the program
{
// Create Vertex Array Object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint shaderProgram = initShaders();
	initProjmatrix();
	initVertexAttribs();

	uniView = glGetUniformLocation(shaderProgram, "view");

	GLint light = glGetUniformLocation(shaderProgram, "LIGHT_DIR");
	glUniform3fv(light, 1, value_ptr(GLOBAL_LIGHT_DIRECTION));

	glEnable(GL_DEPTH_TEST);

	initTerrain();

	std::cout << "init done\n";
}

float ang = 0.f;

void tick() {
	ang += 0.05f;
	if(ang > 2 * M_PI) ang -= 2 * M_PI;

	
	camerapos = camerapos + (vec3(0,0, -10. * (float) up) * camerarotation);
	camerapos = camerapos + (vec3(0,0, 10. * (float) down) * camerarotation);
	camerapos = camerapos + (vec3(10. * (float) right, 0, 0) * camerarotation);
	camerapos = camerapos + (vec3(-10. * (float) left, 0, 0) * camerarotation);

	std::cout << std::setprecision(3) << std::fixed
		<< "(" << camerapos.x << "," << camerapos.y << "," << camerapos.z << ")";
	vec3 facing = camerarotation * vec3(0, 0, 1);
	std::cout 
		<< "(" << facing.x << "," << facing.y << "," << facing.z << ")" << std::endl;
}

// Called at the start of the program, after a glutPostRedisplay() and during idle
// to display a frame
void display()
{

	tick();

	glBindVertexArray(vao);

	// Clear the screen to black
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 view(1.f);
	view = translate(view, -camerapos);
	view = mat4_cast(camerarotation) * view;

	glUniformMatrix4fv(uniView, 1, GL_FALSE, value_ptr(view));

	drawTerrain();

	glutSwapBuffers();
}

void idle() {
	glutPostRedisplay();
}

void keyboardDownFunc(unsigned char c, int x, int y) {
	std::cout << "down:" << c << std::endl;

	switch(c | 0x20) {
	case 'a': left = true; break;
	case 'w': up = true; break;
	case 's': down = true; break;
	case 'd': right = true; break;
	}
}

void keyboardUpFunc(unsigned char c, int x, int y) {
	std::cout << "up  :" << c << std::endl;

	switch(c | 0x20) {
	case 'a': left = false; break;
	case 'w': up = false; break;
	case 's': down = false; break;
	case 'd': right = false; break;
	}
}

void mouseMoveFunc(int x, int y) {
	int dx = x - mx;
	int dy = y - my;

	quat rot = normalize(quat(1, 0.003*dy, 0.003*dx, 0));
	camerarotation = rot * camerarotation;

	mx = x;
	my = y;
}

int main(int argc, char **argv)
{
	initializeGLWindow(argc, argv, 800, 600);

	// passes reshape and display functions to the OpenGL machine for callback
	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(keyboardDownFunc);
	glutKeyboardUpFunc(keyboardUpFunc);
	glutMotionFunc(mouseMoveFunc);
	glutPassiveMotionFunc(mouseMoveFunc);

	init();

	// Starts the program.
	glutMainLoop();
	return 0;
}
