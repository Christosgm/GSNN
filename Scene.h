#pragma once

#include "utils/Header.h"
#include "gaussians/SplatData.h"
#include "gaussians/PointCloud.h"
#include "camera/CamerasLoader.h"

CamerasLoader* cameras; 
Camera* camera;

class Scene
{
private:
	//Window parameters
	GLFWwindow* window;
	int WIDTH;
	int HEIGHT;

	int MAJOR_VERSION;
	int MINOR_VERSION;

	glm::mat4 ViewProjection;

	PointCloud* point_cloud;
	Shader* shader_normal;
	Shader* shader_peeling;
	Shader* shader_quad;
	Shader* shader_xtract;
	ShaderManager* sm;

	void initGLFW();
	void initWindow(const char* title, bool resizable);
	void initGLEW();
	void initOpenGLOptions();
	void initMatrices();
 
	void setUniforms();

public:
	Scene(const char* title,
		  const int width,
		  const int height,
		  const int GLmajorVer,
		  const int GLminorVer,
		  bool resizable);

	virtual ~Scene();

	int getWindowShouldClose();
	void setWindowShouldClose();

	void getInput();
	void update();
 
	void render();
};