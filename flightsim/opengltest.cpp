#include "openglheaders.hpp"

#include <ctime>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>
#include <iomanip>

#include "openglshaders.hpp"
#include "openglutil.hpp"

GLint uniTrans;
GLint uniView;
GLuint vao;
GLuint vbo;
GLuint ebo0, ebo1;

vec3 camerapos(0, 0, 3);
quat camerarotation(1,0,0,0);

bool left, right, up, down;
int mx, my;

void init() // Called before main loop to set up the program
{
// Create Vertex Array Object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create a Vertex Buffer Object and copy the vertex data to it
	glGenBuffers(1, &vbo);

	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // 000
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, // 001
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // 010
		 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, // 100
		-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // 011
		 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // 101
		 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, // 110
		 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, // 111
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Create an element array
	glGenBuffers(1, &ebo0);
	glGenBuffers(1, &ebo1);

	GLuint elements0[] = {
		0, 1, 2,
		4, 1, 2,
		0, 1, 3,
		5, 1, 3,
		0, 2, 3,
		6, 2, 3,
	};
	GLuint elements1[] = {
		7, 4, 5,
		1, 4, 5,
		7, 4, 6,
		2, 4, 6,
		7, 5, 6,
		3, 5, 6,
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo0);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements0), elements0, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements1), elements1, GL_STATIC_DRAW);

	GLuint shaderProgram = initShaders();
	initProjmatrix();

	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);

	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	uniView = glGetUniformLocation(shaderProgram, "view");

	glEnable(GL_DEPTH_TEST);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

std::cout << "init done\n";
}

float ang = 0.f;

void tick() {
	ang += 0.05f;
	if(ang > 2 * M_PI) ang -= 2 * M_PI;

	
	camerapos = camerapos + (vec3(0,0, -0.1 * (float) up) * camerarotation);
	camerapos = camerapos + (vec3(0,0, 0.1 * (float) down) * camerarotation);
	camerapos = camerapos + (vec3(0.1 * (float) right, 0, 0) * camerarotation);
	camerapos = camerapos + (vec3(-0.1 * (float) left, 0, 0) * camerarotation);

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
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
 // Clear the screen to black
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 trans(1.f);
	trans = rotate(trans, ang, glm::vec3(.0f, 1.f, 0.f));

	mat4 view(1.f);
	view = translate(view, -camerapos);
	view = mat4_cast(camerarotation) * view;

	glUniformMatrix4fv(uniTrans, 1, GL_FALSE, value_ptr(trans));
	glUniformMatrix4fv(uniView, 1, GL_FALSE, value_ptr(view));

	// Draw a rectangle from the 2 triangles using 6 indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo0);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo1);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

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
	initializeGLWindow(argc, argv, 1024, 1024);

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
