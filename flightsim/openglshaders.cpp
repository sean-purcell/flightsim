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
    INV "vec3 normal;"
    OUTV "vec3 Color;"
    OUTV "vec3 Normal;"
    "uniform mat4 proj;"
    "uniform mat4 view;"
    "void main() {"
    "   Color = color;"
    "   Normal = normal;"
    "   gl_Position = proj * view * vec4(position, 1.0);"
    "}";

const GLchar* fragmentSource =
    VERSION
    INF "vec3 Color;"
    INF "vec3 Normal;"
    OUTF "vec4 outColor;"
    "uniform vec3 LIGHT_DIR;"
    "void main() {"
    "    vec3 tmp = vec3(0.4, 0.4, 0.4) + vec3(0.6, 0.6, 0.6) * abs(dot(LIGHT_DIR, Normal));"
    "    outColor = vec4(tmp * Color, 1.0);"
    "    outColor = vec4(1.0,1.0,1.0,1.0);"
    "}";

