#include <iostream>

#include "hudrender.hpp"
#include "openglutil.hpp"

static GLuint vbo, ebo;

void drawHud(vec3 pos, quat facing, vec3 vel) {
	hudMode(true);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	updateHudVertexAttribs();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	hudMode(false);
}

void initHud() {
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	GLfloat vertices[] = {
		-10.f, -10.f, -20.f, 0.f, 0.f,
		-10.f,  10.f, -20.f, 0.f, 1.f,
		 10.f, -10.f, -20.f, 1.f, 0.f,
		 10.f,  10.f, -20.f, 1.f, 1.f,
	};

	GLushort indices[] = {
		0, 1, 2,
		3, 1, 2,
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

