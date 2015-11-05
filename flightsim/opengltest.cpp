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

const int chunksAround = 20;

const vec3 GLOBAL_LIGHT_DIRECTION = normalize(vec3(0, 1, 0.2));
const vec3 SKY_COLOR = vec3(0.39f, 0.39f, 0.94f);
const vec3 FOG_COLOR = vec3(0.75f, 0.75f, 0.75f);

// min_alt = (scaling factor for amplitude) * (maximum value of amplitude noise) *
//   (sum of geometric series for maximum value of persistence)
const float MIN_ALT = 160 * 2.1 * 1 / (1 - 0.7);
const float HORIZON_DIST = chunksAround * 10 * CHUNKWIDTH;

void drawTerrain() {
	TerrainChunk *iter = chunks;
	while(iter) {
		iter->draw();
		iter = iter->next;
	}
}

GLuint horizonVbo = 0;
GLuint horizonEbo = 0;
void drawHorizon() {
	GLfloat vertices[108];

	GLubyte indices[] = {
		0, 8, 5,
		0, 9, 5,
		1, 9, 7,
		1, 11, 7,
		3, 11, 6,
		3, 10, 6,
		2, 10, 4,
		2, 8, 4,
	};
	if(horizonVbo == 0 && horizonEbo == 0) {
		glGenBuffers(1, &horizonVbo);
		glGenBuffers(1, &horizonEbo);

		glBindBuffer(GL_ARRAY_BUFFER, horizonVbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, horizonEbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	} else {
		glBindBuffer(GL_ARRAY_BUFFER, horizonVbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, horizonEbo);
	}
	updateVertexAttribs();

	static int dx[] = {0, 0, 0, 0, -1, 1, -1, 1, 0, 1, -1, 0};
	static int dz[] = {0, 0, 0, 0, -1, -1, 1, 1, -1, 0, 0, 1};

	int cx = (int)(camerapos.x / CHUNKWIDTH);
	int cz = (int)(camerapos.z / CHUNKWIDTH);

	vec3 corners[] = {
		vec3((cx - chunksAround) * CHUNKWIDTH, -MIN_ALT, (cz - chunksAround) * CHUNKWIDTH),
		vec3((cx + chunksAround+1) * CHUNKWIDTH, -MIN_ALT, (cz - chunksAround) * CHUNKWIDTH),
		vec3((cx - chunksAround) * CHUNKWIDTH, -MIN_ALT, (cz + chunksAround+1) * CHUNKWIDTH),
		vec3((cx + chunksAround+1) * CHUNKWIDTH, -MIN_ALT, (cz + chunksAround+1) * CHUNKWIDTH),
	};

	for(int i = 0; i < 4; i++) {
		vec3 v = corners[i];
		vertices[i * 9 + 0] = v.x;
		vertices[i * 9 + 1] = v.y;
		vertices[i * 9 + 2] = v.z;
	}

	for(int i = 4; i < 12; i++) {
		vec3 v = vec3(dx[i], 0, dz[i]);
		v = v * HORIZON_DIST;
		v = v + corners[i % 4];

		vertices[i * 9 + 0] = v.x;
		vertices[i * 9 + 1] = v.y;
		vertices[i * 9 + 2] = v.z;
	}

	for(int i = 0; i < 12; i++) {
		vertices[i * 9 + 3] = GLOBAL_LIGHT_DIRECTION.x;
		vertices[i * 9 + 4] = GLOBAL_LIGHT_DIRECTION.y;
		vertices[i * 9 + 5] = GLOBAL_LIGHT_DIRECTION.z;

		vertices[i * 9 + 6] = FOG_COLOR.x;
		vertices[i * 9 + 7] = FOG_COLOR.y;
		vertices[i * 9 + 8] = FOG_COLOR.z;
	}
	for(int i = 0; i < 12; i++) {
		std::cout << i << ": (" << vertices[i * 9 + 0] << "," << vertices[i * 9 + 1] << "," << vertices[i * 9 + 2] << ")" << std::endl;
	}

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_BYTE, 0);
}

void updateTerrain() {
	TerrainChunk *nchunks = chunkmanager.getNewChunks(camerapos.x, camerapos.z, chunksAround);
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
			TerrainChunk *next = (*cur)->next;
			delete *cur;
			*cur = next;
		} else {
			cur = &(*cur)->next;
			len++;
		}
	}
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
	GLint sky = glGetUniformLocation(shaderProgram, "FOG_COLOR");
	glUniform3fv(sky, 1, value_ptr(FOG_COLOR));

	GLint horizon = glGetUniformLocation(shaderProgram, "horizon");
	glUniform1f(horizon, 0.8f * chunksAround * CHUNKWIDTH);

	glEnable(GL_DEPTH_TEST);

	initTerrain();

	std::cout << "init done\n";
}

float ang = 0.f;

void tick() {
	ang += 0.05f;
	if(ang > 2 * M_PI) ang -= 2 * M_PI;

	
	camerapos = camerapos + (vec3(0,0, -50. * (float) up) * camerarotation);
	camerapos = camerapos + (vec3(0,0, 50. * (float) down) * camerarotation);
	camerapos = camerapos + (vec3(50. * (float) right, 0, 0) * camerarotation);
	camerapos = camerapos + (vec3(-50. * (float) left, 0, 0) * camerarotation);

	std::cout << std::setprecision(3) << std::fixed
		<< "(" << camerapos.x << "," << camerapos.y << "," << camerapos.z << ")";
	vec3 facing = camerarotation * vec3(0, 0, 1);
	std::cout 
		<< "(" << facing.x << "," << facing.y << "," << facing.z << ")" << std::endl;

	updateTerrain();
}

// Called at the start of the program, after a glutPostRedisplay() and during idle
// to display a frame
void display()
{

	tick();

	glBindVertexArray(vao);

	glClearColor(SKY_COLOR.r, SKY_COLOR.g, SKY_COLOR.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 view(1.f);
	view = translate(view, -camerapos);
	view = mat4_cast(camerarotation) * view;

	glUniformMatrix4fv(uniView, 1, GL_FALSE, value_ptr(view));

	drawTerrain();
	drawHorizon();

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
