#include <GL/glew.h>

class FrameBuffer
{
    private:
        GLuint id;
        GLuint colorTex;
        GLuint depthTex;

        int width;
        int height;
    public:
        FrameBuffer(int width, int height)
        {
            this->width = width;
            this->height = height;
            glGenFramebuffers(1, &this->id);

            bind();
                glGenTextures(1, &colorTex);
                glBindTexture(GL_TEXTURE_2D, colorTex);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);

                glGenTextures(1, &depthTex);
                glBindTexture(GL_TEXTURE_2D, depthTex);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
            unbind();
        }

        ~FrameBuffer()
        {
            GLuint textures[2] = {colorTex, depthTex};
			glDeleteTextures(2, textures);
        }

        void bind()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, this->id);
        }

        void unbind()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void clear(GLfloat depth, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
        {
            glClearDepth(depth);
			glClearColor(r, g, b, a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        void printDepthBuffer(int num)
        {
            GLfloat* depthBuffer;
            depthBuffer = retrieveDepthBuffer(depthTex, width, height);
            for (int i = 0; i < num; i++) std::cout << depthBuffer[0] << " ";
            std::cout << std::endl;
        }

        void clearDepth()
        {
            GLfloat* values = (GLfloat*)calloc(width*height, sizeof(GLfloat));
            glClearBufferfv(this->id, GL_DEPTH_BUFFER, values);
        }

        GLuint getDepthTex()
        {
            return depthTex;
        }

        GLuint getColorTex()
        {
            return colorTex;
        }
};