#include "openglheaders.hpp"

extern int w, h;

void reshape(int w, int h);
void initializeGLWindow(int argc, char **argv, int w, int h);
GLuint initShaders();
void initProjmatrix();
void initVertexAttribs();
void updateVertexAttribs();

