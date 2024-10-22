#include <GL/glew.h>
// #include "Errors.h"

void initTexture(GLuint* id, GLuint unit, int WIDTH, int HEIGHT, GLenum type, GLenum format, GLfloat init_value)
{
    glGenTextures(1, id);
    glBindTexture(type, *id);
    std::vector<GLfloat> data(WIDTH * HEIGHT * 4, init_value);
    glTexImage2D(type, 0, format, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, data.data());
    glBindImageTexture(unit, *id, 0, GL_TRUE, 0, GL_READ_WRITE, format);
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(type, 0);
}

void initDepthTexture(GLuint* id, GLuint unit, int WIDTH, int HEIGHT, GLenum type, GLfloat init_value)
{
    glGenTextures(1, id);
    glBindTexture(type, *id);
    std::vector<GLfloat> data(WIDTH * HEIGHT, init_value);
    glTexImage2D(type, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data.data());
    glBindImageTexture(unit, *id, 0, GL_TRUE, 0, GL_READ_WRITE, GL_DEPTH_COMPONENT);
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(type, 0);
}

void bindSamplerTexture(GLuint id)
{
    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id);
}

void bindTexture(GLuint id, GLuint unit, GLenum format)
{
    glBindTexture(GL_TEXTURE_2D, id);
	glBindImageTexture(unit, id, 0, GL_TRUE, 0, GL_READ_WRITE, format);
}

void unbindTexture()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}