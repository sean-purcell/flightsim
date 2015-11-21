#pragma once

// The OpenGL libraries, make sure to include the GLUT and OpenGL frameworks
#define  GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED

#ifdef __linux__
#  define GLEW_STATIC
#  include <GL/glew.h>
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/glut.h>
#  include <GL/glext.h>
#endif

#ifdef __APPLE__
# include <OpenGL/gl3.h>
# include <OpenGL/gl.h>
# include <GLUT/glut.h>
#endif

#ifdef __MINGW32__
#  define GLEW_STATIC
#  include <GL/glew.h>
#  include <GL/glut.h>
#endif

#include "glmheaders.hpp"

