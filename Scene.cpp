#include "Scene.h"
#include "utils/Options.h"

bool rightIsNotHeld = true;
bool leftIsNotHeld = true;

float currentTime;
float lastTime;

ProgressBar* progress;

std::ofstream file;

Scene::Scene(const char* title, const int WIDTH, const int HEIGHT, const int MAJOR_VERSION, const int MINOR_VERSION, bool resizable)

	:WIDTH(WIDTH),
	HEIGHT(HEIGHT),
	MAJOR_VERSION(MAJOR_VERSION),
	MINOR_VERSION(MINOR_VERSION)
{
	//Initialize window Values
	window = nullptr;

	SplatData sd = SplatData(PLY_MODEL);

	initGLFW();
	initWindow(title, resizable);
	initGLEW();
	initOpenGLOptions();

	#ifdef DEPTH_PEELING
		shader_normal = new Shader("shaders/vertex_dp.glsl", "shaders/fragment_dp.glsl");
		shader_peeling = new Shader("shaders/vertex_dp.glsl", "shaders/fragment_dp2.glsl");
		shader_quad = new Shader("shaders/vertex_quad.glsl", "shaders/fragment_quad.glsl");
		shader_xtract = new Shader("shaders/vertex_dp.glsl", "shaders/fragment_dp3.glsl");
	#else
		shader = new Shader("shaders/vertex_extract.glsl", "shaders/fragment_extract.glsl");
	#endif
	Shader** shaders = (Shader**)malloc(4*sizeof(Shader*));
	shaders[0] = shader_normal;
	shaders[1] = shader_peeling;
	shaders[2] = shader_quad;
	shaders[3] = shader_xtract;
	sm = new ShaderManager(shaders);
	point_cloud = new PointCloud(&sd, sm, WIDTH, HEIGHT);

	initMatrices();
	setUniforms();
}

Scene::~Scene()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Scene::initGLFW()
{
	if (glfwInit() == GLFW_FALSE)
	{
		std::cout << "Error initializing GLFW!" << std::endl;
		glfwTerminate();
	}
}

void Scene::initWindow(const char* title, bool resizable)
{
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MAJOR_VERSION);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,MINOR_VERSION);
	glfwWindowHint(GLFW_RESIZABLE, resizable);

	window = glfwCreateWindow(WIDTH, HEIGHT, title, NULL, NULL);

	if (window == nullptr)
	{
		std::cout << "Error initializing window" << std::endl;
		glfwTerminate();
	}
 
	glfwMakeContextCurrent(window);
}

int Scene::getWindowShouldClose()
{
	return glfwWindowShouldClose(window);
}

void Scene::setWindowShouldClose()
{
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void Scene::initGLEW()
{
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW initialization failed!" << std::endl;
		glfwTerminate();
	}
}

void Scene::initMatrices()
{
	#ifndef EXTRACT
		std::cout << "(" << cameras->getCurrentCameraIndex() << "): " << cameras->getCameraFilename() << std::endl;
	#endif
	camera = cameras->getCurrentCameraView();
	ViewProjection = camera->getViewProjection();
}

void Scene::setUniforms()
{
	sm->setUniforms(camera->getPosition(), 
					glm::vec2(WIDTH, HEIGHT),
					camera->getViewMatrix(),
					camera->getProjectionMatrix(),
					camera->getfx(),
					camera->getfy()
				   );
}  

void Scene::getInput()
{
	glfwPollEvents();

	//CLOSE WINDOW
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) setWindowShouldClose();
	
	//VIEW CONTROL
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && rightIsNotHeld)
	{
		cameras->next();
		std::cout << "\n(" << cameras->getCurrentCameraIndex() << "): " << cameras->getCameraFilename() << std::endl;
		camera = cameras->getCurrentCameraView();
		ViewProjection = camera->getViewProjection();
		rightIsNotHeld = false;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && leftIsNotHeld)
	{
		cameras->prev();
		std::cout << "\n(" << cameras->getCurrentCameraIndex() << "): " << cameras->getCameraFilename() << std::endl;
		camera = cameras->getCurrentCameraView();
		ViewProjection = camera->getViewProjection();
		leftIsNotHeld = false;
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE && !rightIsNotHeld) rightIsNotHeld = true;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE && !leftIsNotHeld) leftIsNotHeld = true;
}

//Scene update 
void Scene::update()
{
	#ifndef EXTRACT
		getInput();
	#endif
	setUniforms();
}

void Scene::initOpenGLOptions()
{
	//DEPTH
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//CULLING
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, WIDTH, HEIGHT);
}

void Scene::render()
{
	#ifdef EXTRACT
		point_cloud->setImageName(cameras->getCameraFilename());
		point_cloud->setIndex(cameras->getCurrentCameraIndex());

		std::string name;
		int img_name = cameras->getCurrentCameraIndex() + 1;
		std::string num = std::to_string(img_name);
		if (img_name < 10) name = "0000" + num;
		else if (img_name < 100) name = "000" + num;
		else name = "00" + num;

		file << name << std::endl;
		progress->update(1);
	#endif
	point_cloud->depthPeeling(10);
	glfwSwapBuffers(window);
	glFlush();

	//Prepare camera for next frame
	#ifdef EXTRACT
		cameras->next();
		camera = cameras->getCurrentCameraView();
		ViewProjection = camera->getViewProjection();
		point_cloud->deleteFeatureTextures();
		point_cloud->initFeatureTextures();

		if (cameras->getCurrentCameraIndex() == 0)
		{
			setWindowShouldClose();
		}
	#endif

	glBindVertexArray(0);
	glUseProgram(0);
}

int main()
{	
	std::cout << "Loading camera configurations..." << std::endl;
	cameras = new CamerasLoader(CAMERAS_CONFIG);
	// cameras->setCameraView(134);
	progress = new ProgressBar(cameras->getNumberOfCameras(), "Data Extracted: ");
	std::cout << "Loaded " << cameras->getNumberOfCameras() << " camera views..." << std::endl;
	int sceneWidth = cameras->getCurrentCameraView()->getWidth();
	int sceneHeight = cameras->getCurrentCameraView()->getHeight();
	if (sceneWidth%2 != 0) sceneWidth = sceneWidth/2 + 1; else sceneWidth/=2; //Fixing width and height based on image (images are scaled to half)
	if (sceneHeight%2 != 0) sceneHeight = sceneHeight/2 + 1; else sceneHeight/=2;
	std::cout << "Size: " << sceneWidth << " " << sceneHeight << std::endl;
	Scene scene("Gaussians Renderer", sceneWidth, sceneHeight, 4, 5, false);

	#ifdef RENDER_TIME
		lastTime = glfwGetTime();
	#endif

	int numberOfFrames = 0;

	#ifdef EXTRACT
		file.open(std::string(FEATURES_PATH)+"imagenames", std::ios::out);
		file << sceneWidth << " " << sceneHeight << std::endl;
		std::cout << "Extracting Data...\n";
	#endif

	//Main loop
	while (!scene.getWindowShouldClose())
	{
		scene.update();
		scene.render();

		#ifdef RENDER_TIME
			currentTime = glfwGetTime();
			numberOfFrames++;
			if (currentTime - lastTime >= 1.0)
			{
				std::cout << "\r" << 1000.0/double(numberOfFrames) << " ms/frame" << std::flush;
				numberOfFrames = 0;
				lastTime += 1.0;
			}
		#endif

		numberOfFrames++;	
	}
	#ifdef EXTRACT
		file.close();
	#endif
	std::cout << std::endl;
	return 0;
}