#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "Camera.h"
#include "../external/json.hpp"

using json = nlohmann::json;

class CamerasLoader
{
    private:

        std::string filename;
        std::vector<std::string> imageNames;
        std::vector<Camera*> cameras;
        int currentCameraIndex = 0;
        float nearPlane;
        float farPlane;

    public:

        CamerasLoader(const char* filename)
        {
            this->filename = filename;
            std::ifstream f(filename);
            json data = json::parse(f);

            for (int i = 0; i < (int)data.size(); i++)
            {
                imageNames.push_back(data[i]["img_name"]);
                int width = data[i]["width"];
                int height = data[i]["height"];
                float fx = data[i]["fx"];
                float fy = data[i]["fy"];
                std::vector<float> position = data[i]["position"];
                std::vector<std::vector<float>> rotation = data[i]["rotation"];

                glm::vec3 camPosition = glm::vec3(position[0], position[1], position[2]);

                glm::mat4 CamConfig = glm::mat4(1.f);
                for (int k = 0; k < 3; k++)
                {
                    for (int l = 0; l < 3; l++)
                    {
                        CamConfig[k][l] = rotation[k][l];
                    }
                }
                for (int k = 0; k < 3; k++)
                {
                    CamConfig[k][3] = position[k];
                }

                glm::mat3 R = glm::mat3(1.f);
                for (int k = 0; k < 3; k++)
                {
                    for (int l = 0; l < 3; l++)
                    {
                        R[k][l] = CamConfig[k][l];
                    }
                }

                glm::mat4 inverseCamConfig = glm::inverse(CamConfig);
                glm::vec3 T = glm::vec3(inverseCamConfig[0][3], inverseCamConfig[1][3], inverseCamConfig[2][3]);
                nearPlane = 0.01f;
                farPlane = 100.f;
                Camera* cam = new Camera(camPosition, R, T, fx, fy, width, height, nearPlane, farPlane);
                cameras.push_back(cam);
            }
        }

        Camera* getCurrentCameraView()
        {
            return cameras[currentCameraIndex];
        }

        Camera* getCameraView(int idx)
        {
            return cameras[idx];
        }

        std::string getCameraFilename()
        {
            return imageNames[currentCameraIndex];
        }

        void next()
        {
            currentCameraIndex++;
            if (currentCameraIndex >= (int)cameras.size()) currentCameraIndex = 0;
        }

        void prev()
        {
            currentCameraIndex--;
            if (currentCameraIndex < 0) currentCameraIndex = cameras.size() - 1;
        }

        void setCameraView(int idx)
        {
            if (idx < 0 || idx > (int)cameras.size()-1) return;
            currentCameraIndex = idx;
        }

        int getCurrentCameraIndex()
        {
            return currentCameraIndex;
        }

        int getNumberOfCameras()
        {
            return cameras.size();
        }

};