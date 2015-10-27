#define GLEW_STATIC

#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <chrono>
#include <cmath>

#include "openglheaders.hpp"

#include "shaders.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Shader sources
const GLchar* vertexSource =
    "#version 120\n"
    "vec2 position;"
    "void main() {"
    "   gl_Position = vec4(position, 0, 1);"
    "}";
const GLchar* fragmentSource =
    "#version 120\n"
    "vec4 outColor;"
    "void main() {"
    "   outColor = vec4(1.0f,1.0f,1.0f,1.0f);"
    "}";

int fov=90;
float ratio=std::tan(fov/2.0);
int screenwidth=800;
int screenheight=600;
sf::Vector2i size(screenwidth, screenheight);

int main() {
	auto t_start = std::chrono::high_resolution_clock::now();
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;

	sf::Window window(sf::VideoMode(800, 600, 32), "OpenGL",
		sf::Style::Titlebar | sf::Style::Close, settings);

#ifndef __APPLE__
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	std::cout << vertexSource << std::endl;
// Create Vertex Array Object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint vbo;
	glGenBuffers(1, &vbo);

	GLfloat vertices[] = {
		0.0f, -1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

	while (window.isOpen())
	{
		sf::Event windowEvent;
		while (window.pollEvent(windowEvent))
		{
			switch (windowEvent.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			}
		}

		// Clear the screen to black
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		glBegin(GL_TRIANGLES);
		glVertex2f(0.0f, 1.0f);
		glVertex2f(1.0f, 0.0f);
		glVertex2f(0.0f, -1.0f);
		glEnd();

		// Swap buffers
		window.display();
	}

	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	glDeleteBuffers(1, &vbo);

	glDeleteVertexArrays(1, &vao);
}

