#include <GL/glew.h>
#include <string>
#include <iostream>

GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

GLenum glCheckFrameBufferCompleteness_(const char *file, int line, GLuint id)
{
    GLenum errorCode = glCheckNamedFramebufferStatus(id, GL_FRAMEBUFFER);
    std::string error;
    switch (errorCode)
    {
        case GL_FRAMEBUFFER_COMPLETE:                       error = "GL_FRAMEBUFFER_COMPLETE"; break;
        case GL_FRAMEBUFFER_UNDEFINED:                      error = "GL_FRAMEBUFFER_UNDEFINED"; break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:          error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:  error = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:         error = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"; break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:         error = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"; break;
        case GL_FRAMEBUFFER_UNSUPPORTED:                    error = "GL_FRAMEBUFFER_UNSUPPORTED"; break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:         error = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"; break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:       error = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"; break;
        case 0:                                             error = "ERROR"; break;
    }
    std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    return errorCode;
}
#define glCheckFrameBufferCompleteness(x) glCheckFrameBufferCompleteness_(__FILE__, __LINE__, x) 