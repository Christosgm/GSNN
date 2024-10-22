#include <iostream>
#include <glm/glm.hpp>
#include <GL/glew.h>

void print_vec3(glm::vec3 v)
{
    std::cout << "[" << v[0] << " " << v[1] << " " << v[2] << "]\n";
}

void print_vec4(glm::vec4 v)
{
    std::cout << "[" << v[0] << " " << v[1] << " " << v[2] << " " << v[3] << "]\n";
}

void printrangearray(GLfloat* f, int n, int m)
{
    std::cout << "[";
    for (int i = n; i < m-1; i++)
    {
        std::cout << f[i] << " ";
    }
    std::cout << f[m-1] << "]" << std::endl;
}

void print_float_array(float* arr, int num_elements)
{
    for (int i = 0; i < num_elements; i++)
    {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

void print_mat4(glm::mat4 m)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            std::cout << m[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void print_mat3(glm::mat3 m)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            std::cout << m[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void print_mat2(glm::mat2 m)
{
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            std::cout << m[i][j] << " ";
        }
        std::cout << std::endl;
    }
}