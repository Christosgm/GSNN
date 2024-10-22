#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "../utils/PrintUtils.h"

class Camera
{
    private:
        glm::vec3 position;
        glm::mat3 R;
        glm::vec3 T;
        glm::mat4 V;
        glm::mat4 P;
        glm::mat4 VP;
        float fx;
        float fy;
        float nearPlane;
        float farPlane;
        int width;
        int height;

        glm::mat4 constructView()
        {
            glm::mat4 view = glm::mat4(1.f);
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    view[i][j] = R[i][j];
                }
            }
            for (int i = 0; i < 3; i++)
            {
                view[3][i] = T[i];
            
            }
            return view;
        }

        glm::mat4 constructProjection()
        {

            float tanHalfFovY = tan((fy / 2));
            float tanHalfFovX = tan((fx / 2));

            float top = tanHalfFovY * nearPlane;
            float bottom = -top;
            float right = tanHalfFovX * nearPlane;
            float left = -right;

            glm::mat4 P = glm::mat4(0.f);

            float z_sign = 1.0;
            P[0][0] = 2.0 * nearPlane / (right - left);
            P[1][1] = 2.0 * nearPlane / (top - bottom);
            P[0][2] = (right + left) / (right - left);
            P[1][2] = (top + bottom) / (top - bottom);
            P[3][2] = z_sign;
            P[2][2] = z_sign * (farPlane + nearPlane) / (farPlane - nearPlane);
            P[2][3] = -(2.0 * farPlane * nearPlane) / (farPlane - nearPlane);
            return glm::transpose(P);
        }
    public:
        Camera(glm::vec3 camPosition, glm::mat3 R, glm::vec3 T, float fx, float fy, int width, int height, float nearPlane, float farPlane)
        {
            this->position = camPosition;
            this->R = R;
            this->T = T;
            this->fx = focal2fov(fx, width);
            this->fy = focal2fov(fy, height);
            this->width = width;
            this->height = height;
            this->nearPlane = nearPlane;
            this->farPlane = farPlane;
            this->V = constructView();
            this->P = constructProjection();
            this->VP = P*V;
        }

        float getfx()
        {
            return this->fx;
        }

        float getfy()
        {
            return this->fy;
        }

        glm::vec3 getTranslation()
        {
            return T;
        }

        glm::vec3 getPosition()
        {
            return position;
        }

        glm::mat3 getRotation()
        {
            return R;
        }

        int getWidth()
        {
            return width;
        }

        int getHeight()
        {
            return height;
        }

        glm::mat4 getViewMatrix()
        {
            return V;
        }

        glm::mat4 getProjectionMatrix()
        {
            return P;
        }

        glm::mat4 getViewProjection()
        {
            return VP;
        }

        static float focal2fov(float focal, float pixels)
        {
            return 2.f * atan(pixels/(2.f * focal));
        }

};      