#pragma once

#include <fstream>
#include <string>
#include <vector>
#include<iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader
{
private:
	GLuint id;

	std::string loadShaderSource(const char* fileName)
	{
		std::string temp = "";
		std::string src = "";

		std::ifstream in_file;

		std::cout << "Loading shader file..." << std::endl;
		//Vertex shader
		in_file.open(fileName); //Open file 

		if (in_file.is_open()) //Read file source line by line into string
		{
			while (getline(in_file, temp))
			{
				src += temp + "\n";
			}
		}
		else
		{
			std::cout << "Error in loading shader file " << fileName << std::endl;
		}
		in_file.close();

		return src;
	}

	GLuint loadShader(GLenum type, const char* fileName)
	{
		char infoLog[512];
		GLint success;

		std::cout << "Creating shader " << fileName << "..." << std::endl;
		GLuint shader = glCreateShader(type);
		std::string str_src = loadShaderSource(fileName);
		const GLchar* src = str_src.c_str();
		glShaderSource(shader, 1, &src, NULL);
		std::cout << "Compiling shader..." << std::endl;
		glCompileShader(shader);

		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "Error in compiling shader " << fileName << std::endl;
			std::cout << infoLog << std::endl;
			exit(-1);
		}

		std::cout << "OK!" << std::endl << std::endl;

		return shader;
	}

	void linkProgram(GLuint vertexShader, GLuint fragmentShader)
	{
		char infoLog[512];
		GLint success;

		id = glCreateProgram();
		glAttachShader(id, vertexShader);
		glAttachShader(id, fragmentShader);

		glLinkProgram(id);

		glGetProgramiv(id, GL_LINK_STATUS, &success); 
		if (!success)
		{
			glGetProgramInfoLog(id, 512, NULL, infoLog);
			std::cout << "Error in linking program " << id << std::endl;
			std::cout << infoLog << std::endl;
			exit(-1);
		}

		glUseProgram(0);
	}

public:
	Shader(const char* vertexFile, const char* fragmentFile)
	{
		std::cout << "Building Shader with files:\n\n" << vertexFile << "\n" << fragmentFile << std::endl << std::endl;
		GLuint vertexShader = 0;
		GLuint fragmentShader = 0;
		
		vertexShader = loadShader(GL_VERTEX_SHADER, vertexFile);
		fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentFile);

		linkProgram(vertexShader, fragmentShader);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	~Shader()
	{
		glDeleteProgram(id);
	}

	void use()
	{
		glUseProgram(id);
	}

	void unuse()
	{
		glUseProgram(0);
	}

	GLuint getID()
	{
		return id;
	}

	void setMat4f(glm::mat4 value, const GLchar* name, GLboolean transpose = GL_FALSE)
	{
		glUniformMatrix4fv(glGetUniformLocation(id, name), 1, transpose, value_ptr(value));
	}

	void setVec2f(GLfloat v1, GLfloat v2, const GLchar* name)
	{
		glUniform2f(glGetUniformLocation(id, name), v1, v2);
	}

	void setVec3f(glm::vec3 position, const GLchar* name)
	{
		glUniform3fv(glGetUniformLocation(id, name), 1, value_ptr(position));
	}

	void set1f(GLfloat value, const GLchar* name)
	{
		glUniform1f(glGetUniformLocation(id, name), value);
	}

	void set1i(GLuint value, const GLchar* name)
	{
		glUniform1i(glGetUniformLocation(id, name), value);
	}
};

enum ShaderType 
{
  NORMAL,
  DEPTH_PEEL,
  QUAD,
  XTRACT_PEEL
};

class ShaderManager
{
    private:
        Shader** shaders;
        ShaderType type;

		//Shader stuff
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 perspective;
		glm::vec3 cameraPosition;
		glm::vec2 viewPortSize;
		float fx;
		float fy;
    public:
        ShaderManager(Shader** shaders)
        {
            this->shaders = shaders;
            this->type = NORMAL;
        }

        void use()
        {
            shaders[type]->use();
        }

        void unuse()
        {
            shaders[type]->unuse();
        }

        void change(ShaderType type)
        {
            this->type = type;
        }

        Shader* getShader()
        {
            return shaders[type];
        }

		void setUniforms(glm::vec3 cameraPosition, glm::vec2 viewPortSize, glm::mat4 view, glm::mat4 perspective, float fx, float fy)
		{
			this->cameraPosition = cameraPosition;
			this->viewPortSize = viewPortSize;
			this->model = model;
			this->view = view;
			this->perspective = perspective;
			this->fx = fx;
			this->fy = fy;
		}

		void sendUniforms()
		{
			shaders[type]->setVec3f(cameraPosition, "camera_position");
			shaders[type]->setVec2f(viewPortSize.x, viewPortSize.y, "viewportSize");
			shaders[type]->setMat4f(view, "view");
			shaders[type]->setMat4f(perspective, "perspective");
			shaders[type]->setMat4f(view, "V");
			shaders[type]->setMat4f(perspective, "P");
			shaders[type]->set1f(fx, "fx");
			shaders[type]->set1f(fy, "fy");
		}
};
