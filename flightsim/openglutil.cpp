#include <cstdio>
#include <iostream>
#include <vector>

#include "openglheaders.hpp"
#include "openglshaders.hpp"
#include "openglutil.hpp"

static GLuint shader;
static GLint projloc;
static mat4 proj;

static int w, h;

void reshape(int _w, int _h) {
	w = _w; h = _h;
	glViewport(0, 0, w, h);
	initProjmatrix();
}

void initializeGLWindow(int argc, char **argv, int _w, int _h) {
	glutInit(&argc, argv); // Initializes glut

	// Sets up a double buffer with RGBA components and a depth component
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA
#ifdef __APPLE__
	 | GLUT_3_2_CORE_PROFILE
#endif
	);

	glutInitWindowSize(w = _w, h = _h);

	// Sets the window position to the upper left
	glutInitWindowPosition(0, 0);

	// Creates a window using internal glut functionality
	glutCreateWindow("Flightsim");

#ifdef __linux__
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	std::printf("%s\n%s\n", 
		glGetString(GL_RENDERER),  // e.g. Intel HD Graphics 3000 OpenGL Engine
		glGetString(GL_VERSION)    // e.g. 3.2 INTEL-8.0.61
		);

	// passes reshape and display functions to the OpenGL machine for callback
	glutReshapeFunc(reshape);

	glutIgnoreKeyRepeat(1);
	glutSetCursor(GLUT_CURSOR_NONE);
}

GLuint initShaders() {
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
		std::cout << "VShader did not compile:\n";
		std::cout << &errorLog[0] << std::endl;
		exit(1);
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
		std::cout << "FShader did not compile:\n";
		std::cout << &errorLog[0] << std::endl;
		exit(1);
	}

	// Link the vertex and fragment shader into a shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	projloc = glGetUniformLocation(shaderProgram, "proj");

	return shader = shaderProgram;
}

void initProjmatrix() {
	proj = infinitePerspective(45.f, w / (float)h, .1f);

	glUniformMatrix4fv(projloc, 1, GL_FALSE, value_ptr(proj));
}

