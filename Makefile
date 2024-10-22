all:
	g++ -Wall Scene.cpp gaussians/SplatData.cpp -lGL -lGLEW -lglfw;
	./a.out;
debug:
	g++ -Wall -g Scene.cpp gaussians/SplatData.cpp -lGL -lGLEW -lglfw;