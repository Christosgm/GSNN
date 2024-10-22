#include <fstream>
#include <iostream>
#include <GL/glew.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>

void saveFeatues(std::string filename, std::vector<glm::vec4> v1,
                                       std::vector<glm::vec4> v2,
                                       std::vector<glm::vec4> v3,
                                       std::vector<glm::vec4> v4,
                                       std::vector<glm::vec4> v5,
                                       std::vector<glm::vec4> v6,
                                       std::vector<glm::vec4> v7,
                                       std::vector<glm::vec4> v8,
                                       std::vector<glm::vec4> v9
                                       )
{
    std::ofstream file;
    file.open(filename, std::ios::out);

    for (unsigned long int i = 0; i < v1.size(); i++)
    {
        file << v1[i].r << " " << v1[i].g << " " << v1[i].b << " " <<
                v2[i].r << " " << v2[i].g << " " << v2[i].b << " " <<
                v3[i].r << " " <<
                v4[i].r << " " <<  
                v5[i].r << " " <<
                v6[i].r << " " <<
                v7[i].r << " " <<
                v8[i].r << " " <<
                v9[i].r << std::endl;
    }
    file.close();
}

void saveScreen(std::string filename, int width, int height)
{
    
    GLfloat* screen = new GLfloat[4 * width * height];
    glReadPixels(0, 0, width, height, GL_RGBA, GL_FLOAT, screen);
    glFinish();
    std::ofstream file;
    file.open(filename, std::ios::out);
    for (int i = 0; i < 4 * width * height; i+=4)
    {
        file << screen[i] << " " << screen[i+1]  << " " << screen[i+2] << std::endl;
    }

    file.close();
}

std::vector<glm::vec3> createAvgRGB(GLfloat* data, int size)
{
    std::vector<glm::vec3> rgb;
    for (int i = 0; i < size; i+=4)
    {
        float count = data[i+3];
        if (count == 0) 
        {
            rgb.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
            continue;
        }
        float r = data[i]/count;
        float g = data[i+1]/count;
        float b = data[i+2]/count;
        rgb.push_back(glm::vec3(r, g, b));
    }
    delete[] data;
    return rgb;
}

std::vector<float> createAvgValue(GLfloat* data, int size)
{
    std::vector<float> vals;
    for (int i = 0; i < size; i+=4)
    {
        float count = data[i+3];
        if (count == 0) 
        {
            vals.push_back(0.0f);
            continue;
        }
        float r = data[i]/count;
        vals.push_back(r);
    }
    delete[] data;
    return vals;
}

std::vector<glm::vec3> createv3vector(GLfloat* data, int size)
{
    std::vector<glm::vec3> rgb;
    for (int i = 0; i < size; i+=4)
    {
        rgb.push_back(glm::vec3(data[i], data[i+1], data[i+2]));
    }
    delete[] data;
    return rgb;
}

std::vector<glm::vec4> createv4vector(GLfloat* data, int size)
{
    std::vector<glm::vec4> rgb;
    for (int i = 0; i < size; i+=4)
    {
        rgb.push_back(glm::vec4(data[i], data[i+1], data[i+2], data[i+3]));
    }
    delete[] data;
    return rgb;
}

std::vector<float> createfloatvector(GLfloat* data, int size)
{
    std::vector<float> vals;
    for (int i = 0; i < size; i+=4)
    {
        vals.push_back(data[i]);
    }
    delete[] data;
    return vals;
}

std::vector<glm::vec3> retrieveAVG(GLuint id, int WIDTH, int HEIGHT, int channels)
{
    GLfloat* data = new GLfloat[WIDTH * HEIGHT * channels];
	glGetTextureImage(id, 0, GL_RGBA, GL_FLOAT, WIDTH * HEIGHT * channels * sizeof(GLfloat), data);
	return createAvgRGB(data, WIDTH*HEIGHT*4);
}

std::vector<float> retrieveAVGVal(GLuint id, int WIDTH, int HEIGHT, int channels)
{
    GLfloat* data = new GLfloat[WIDTH * HEIGHT * channels];
	glGetTextureImage(id, 0, GL_RGBA, GL_FLOAT, WIDTH * HEIGHT * channels * sizeof(GLfloat), data);
	return createAvgValue(data, WIDTH*HEIGHT*4);
}

std::vector<glm::vec3> retrieve(GLuint id, int WIDTH, int HEIGHT, int channels)
{
    GLfloat* data = new GLfloat[WIDTH * HEIGHT * channels];
	glGetTextureImage(id, 0, GL_RGBA, GL_FLOAT, WIDTH * HEIGHT * channels * sizeof(GLfloat), data);
	return createv3vector(data, WIDTH*HEIGHT*4);
}

std::vector<glm::vec4> retrieveFull(GLuint id, int WIDTH, int HEIGHT, int channels)
{
    GLfloat* data = new GLfloat[WIDTH * HEIGHT * channels];
	glGetTextureImage(id, 0, GL_RGBA, GL_FLOAT, WIDTH * HEIGHT * channels * sizeof(GLfloat), data);
	return createv4vector(data, WIDTH*HEIGHT*4);
}

std::vector<float> retrieveVal(GLuint id, int WIDTH, int HEIGHT, int channels)
{
    GLfloat* data = new GLfloat[WIDTH * HEIGHT * channels];
	glGetTextureImage(id, 0, GL_RGBA, GL_FLOAT, WIDTH * HEIGHT * channels * sizeof(GLfloat), data);
	return createfloatvector(data, WIDTH*HEIGHT*4);
}

GLfloat* retrieveDepthBuffer(GLuint id, int WIDTH, int HEIGHT)
{
    GLfloat* data = new GLfloat[WIDTH * HEIGHT];
    glReadPixels(0, 0, WIDTH, HEIGHT, GL_DEPTH_COMPONENT, GL_FLOAT, data);
    GLfloat* data4 = new GLfloat[WIDTH * HEIGHT *4];
	for (int i = 0; i < WIDTH*HEIGHT; i++)
    {
        data4[i] = data[i];
        data4[i+1] = data[i];
        data4[i+2] = data[i];
        data4[i+3] = data[i];
    }
	return data4;
}

void vector2array8(unsigned char* target, std::vector<glm::vec3> source)
{
    for (long unsigned int i = 0; i < source.size(); i++)
    {
        target[3*i] = source[i].r * 255;
        target[3*i + 1] = source[i].g * 255;
        target[3*i + 2] = source[i].b * 255;
    }
}
