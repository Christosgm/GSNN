#include <GL/glew.h>
#include <vector>

template<typename T>
GLuint generatePointsSSBO(const std::vector<T> &points)
{
    GLuint ret = GL_NONE;
    glCreateBuffers(1, &ret);
    glNamedBufferStorage(ret, sizeof(T) * points.size(), points.data(), GL_MAP_READ_BIT);

    return ret;
};