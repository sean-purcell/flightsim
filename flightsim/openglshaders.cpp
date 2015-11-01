#include "openglshaders.hpp"

#ifdef __linux__
#define INV "attribute "
#define OUTV "varying "
#define INF "varying "
#define OUTV ""
#define VERSION "#version 120\n"
#endif

#ifdef __APPLE__
#define INV "in "
#define OUTV "out "
#define INF "in "
#define OUTF "out "
#define VERSION "#version 150 core\n"
#endif

// Shader sources
const GLchar* vertexSource =
    VERSION
    INV "vec3 position;"
    INV "vec3 color;"
    INV "float s;"
    OUTV "vec3 Color;"
    "uniform mat4 trans;"
    "uniform mat4 proj;"
    "uniform mat4 view;"
    "void main() {"
    "   Color = color;"
    "   gl_Position = proj * view * trans * vec4(position, 1.0);"
    "}";

const GLchar* fragmentSource =
    VERSION
    INF "vec3 Color;"
    OUTF "vec4 outColor;"
    "void main() {"
    "    outColor = vec4(Color, (3 - Color.x - Color.y - Color.z) / 3);"
    "}";

