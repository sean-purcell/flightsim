#include "openglheaders.hpp"

#include <ctime>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>

#include "openglshaders.hpp"

// This is just an example using basic glut functionality.
// If you want specific Apple functionality, look up AGL

std::chrono::high_resolution_clock::time_point t_start;
GLint uniTrans;
GLuint vao;
GLuint vbo;
GLuint ebo0, ebo1;

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

	// Create and compile the vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	GLint success = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if(success != GL_TRUE) {
		GLint logSize = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logSize);
		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(logSize);
		glGetShaderInfoLog(vertexShader, logSize, &logSize, &errorLog[0]);
		std::cout << &errorLog[0] << std::endl;
	}

	// Create and compile the fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	success = 0;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if(success != GL_TRUE) {
		GLint logSize = 0;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logSize);
		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(logSize);
		glGetShaderInfoLog(fragmentShader, logSize, &logSize, &errorLog[0]);
		std::cout << &errorLog[0] << std::endl;
	}

	// Link the vertex and fragment shader into a shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);

	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	uniTrans = glGetUniformLocation(shaderProgram, "trans");
	glm::mat4 trans(1.f);
	trans = rotate(trans, 1.f, glm::vec3(1.0f, 0.f, 0.f));
	glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));

	GLint uniView = glGetUniformLocation(shaderProgram, "view");
	glm::mat4 view = glm::lookAt(
		glm::vec3(0.f, 2.f, -2.f),
	glm::vec3(0.f, 0.f, 0.f),
	glm::vec3(0.f, 1.f, 0.f)
	);
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	glm::mat4 proj = glm::infinitePerspective(glm::radians(45.0f), 1.0f, 1.0f);
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	//glEnable(GL_DEPTH_TEST);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

std::cout << "init done\n";
}

float ang = 0.f;

// Called at the start of the program, after a glutPostRedisplay() and during idle
// to display a frame
void display()
{

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
 // Clear the screen to black
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 trans(1.f);
	//trans = translate(trans, glm::vec3(0.f,-1.f,4.f));
	trans = rotate(trans, ang, glm::vec3(.0f, 1.f, 0.f));
	//trans = rotate(trans, (float)M_PI/2, glm::vec3(1.f,0.f,0.f));

	ang += 0.05f;
	//if(ang > 2 * M_PI) ang -= 2 * M_PI;

	glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));

	
	// Draw a rectangle from the 2 triangles using 6 indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo0);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo1);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

	glutSwapBuffers();
}

// Called every time a window is resized to resize the projection matrix
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-0.1, 0.1, -float(h)/(10.0*float(w)), float(h)/(10.0*float(w)), 0.5, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyboardDownFunc(unsigned char c, int x, int y) {
	std::cout << "down:" << c << std::endl;
}

void keyboardUpFunc(unsigned char c, int x, int y) {
	std::cout << "up  :" << c << std::endl;
}

int main(int argc, char **argv)
{

	glutInit(&argc, argv); // Initializes glut

	// Sets up a double buffer with RGBA components and a depth component
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA
#ifdef __APPLE__
	 | GLUT_3_2_CORE_PROFILE
#endif
	);

	// Sets the window size to 512*512 square pixels
	glutInitWindowSize(512, 512);

	// Sets the window position to the upper left
	glutInitWindowPosition(0, 0);

	// Creates a window using internal glut functionality
	glutCreateWindow("Hello!");

#ifdef __linux__
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	// passes reshape and display functions to the OpenGL machine for callback
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);
std::printf("%s\n%s\n", 
		glGetString(GL_RENDERER),  // e.g. Intel HD Graphics 3000 OpenGL Engine
		glGetString(GL_VERSION)    // e.g. 3.2 INTEL-8.0.61
		);

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(keyboardDownFunc);
	glutKeyboardUpFunc(keyboardUpFunc);

	init();

	// Starts the program.
	glutMainLoop();
	return 0;
}
