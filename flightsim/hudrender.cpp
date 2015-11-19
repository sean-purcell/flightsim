#include <iostream>
#include <vector>

#include "inc/lodepng.h"
#include "hudrender.hpp"
#include "openglutil.hpp"
#include "biome-processor.hpp"

static GLuint vbo, ebo;
static GLuint tex;

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
		-20.f, -4.f, -100.f, 0.f, 1.f,
		-20.f,  4.f, -100.f, 0.f, 0.f,
		 20.f, -4.f, -100.f, 1.f, 1.f,
		 20.f,  4.f, -100.f, 1.f, 0.f,
	};

	GLushort indices[] = {
		0, 1, 2,
		3, 1, 2,
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	/* load and buffer the texture */
	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	const char *filename = "resources/text.png";
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filename);
	if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
	initTextureUniform();
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	vec4 hudColor = getHudColor();
	initHudColor(hudColor);
	std::cout << hudColor.r << " " << hudColor.g << " " << hudColor.b << " " << hudColor.a << std::endl;
}

