#include <GL/glew.h>

float quad[] = {
    // positions   // texture coordinates
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

class Quad
{
    private:
        GLuint VAO;
        GLuint VBO;
    public:
        Quad()
        {
            glGenVertexArrays(1, &this->VAO);
			glGenBuffers(1, &this->VBO);
			glBindVertexArray(this->VAO);
			glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0); // Vertex position
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

			glEnableVertexAttribArray(1); // Texture coordinates
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
            glBindVertexArray(0);
        }

        GLuint getVAO()
        {
            return this->VAO;
        }

        GLuint getVBO()
        {
            return this->VBO;
        }
};