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
    OUTV "float dist;"
    OUTV "vec3 Position;"
    "uniform mat4 proj;"
    "uniform mat4 view;"
    "void main() {"
    "   Color = color;"
    "   Normal = normal;"
    "   gl_Position = proj * view * vec4(position, 1.0);"
    "   dist = sqrt(gl_Position.z * gl_Position.z + gl_Position.x * gl_Position.x + gl_Position.y * gl_Position.y);"
    "   Position = position;"
    "}";

const GLchar* fragmentSource =
    VERSION
    INF "vec3 Color;"
    INF "vec3 Normal;"
    INF "float dist;"
    INF "vec3 Position;"
    OUTF "vec4 outColor;"
    "uniform vec3 LIGHT_DIR;"
    "uniform vec3 FOG_COLOR;"
    "uniform float horizon;"
    "uniform float horizoncoeff;"
    "float coeff(float x) {"
    "    return pow(x, 2) * horizoncoeff;"
    "}"
    "float sigmoid(float x) {"
    "    return 1 / (1 + pow(2.718, -5 * x));"
    "}"
    "void main() {"
    "    vec3 one = vec3(1, 1, 1);"
    "    vec3 tmp = 0.5 * one + 0.5 * one * (1.2 * sigmoid(Position.y / 480));"
    "    float alpha = coeff(dist);"
    "    alpha = min(alpha, 1);"
    "    outColor = vec4(alpha * FOG_COLOR + (1-alpha) * tmp * Color, 1.0f);"
    "}";

