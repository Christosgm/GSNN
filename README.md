# Naive OpenGL Renderer for Gaussian Splats
Gaussian Splats naive renderer from trained .ply splat files using the [3D Gaussian Splatting for Real-Time Radiance Field Rendering](https://github.com/graphdeco-inria/gaussian-splatting/tree/main) method.

The [happly](https://github.com/nmwsharp/happly) header by [nmwsharp](https://github.com/nmwsharp/) is used to read the .ply files.

The [json](https://github.com/nlohmann/json) header by [nlohmann](https://github.com/nlohmann/) is used to read the cameras.json files.

[#]: <The [stb_image_write](https://github.com/nothings/stb) header is used for image writing.>

This implementation is loosely based on the [Rasterizing Gaussian Splats (The CUDA-less Way)](https://blog.42yeah.is/rendering/opengl/2023/12/20/rasterizing-splats.html) blog post by [42yeah](https://github.com/42yeah) and the ```SIBR_viewers``` rasterizers from the [3D Gaussian Splatting for Real-Time Radiance Field Rendering](https://github.com/graphdeco-inria/gaussian-splatting/tree/main) GitHub page.

## Dependencies (Linux)

GLEW, GLFW and GLM

Ubuntu dependencies installation example: ```sudo apt install libglew-dev libglfw3 libglm-dev```

# Renderer
## How to run
Compile the project using ```make```.

It should compile and automatically run ```a.out```.

Alternatively, you can compile using ```g++ Scene.cpp gaussians/SplatData.cpp -lGL -lGLEW -lglfw``` and then run ```a.out```.

In the ```Options.h``` file you can specify the following parameters:

- **PLY_MODEL:** File name of the trained .ply model.
- **CAMERAS_CONFIG:** File name of the cameras configurations of the model.
- **FEATURES_PATH:** Directory where to store the extracted features.
- **EXTRACT:** Leave uncommented if you want to extract the features. Otherwise, the inference shader will be used with the pretrained network (TODO).
- **RENDER_TIME:** Leave uncommented if you want to display the render time metric printed in ```ms/frame```.

*Note: The scenes directories must have the structure provided by the 3DGS method including the input images directory.*

## Shaders
- **BAND_COUNT:** How many bands of spherical harmonics to be used (1 - 4) 
    - You must change the corresponding values in all vertex and fragment shaders.

## Extracting Features
In the ```Options.h``` file uncomment the ```EXTRACT``` parameter.

*Note: You must create the directories yourselves for the program to store the features and change ```FEATURES_PATH``` accordingly.*

***IMPOTRANT: Ensure you have enough disk space as feature files tend to occupy quite a bit of space!***

## Controls
Use ```left``` and ```right``` arrow keys to change the camera view (aligned with the ground truth images).

Press ```escape``` key to exit the application.

# Neural Network
In the NN directory there is the ```GSNN.ipynb``` notebook. Run it to train the network and save its state into ```trained_model.pt```.

Python dependencies: Numpy, Pandas, PyTorch, tqdm, PIL, [FLIP](https://github.com/NVlabs/flip/tree/main)

## *TODO*
- [ ] *Create flying camera.*
- [ ] *Create inference shader.*
