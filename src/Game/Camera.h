//
// Camera class heavily based on: https://learnopengl.com/code_viewer.php?type=header&code=camera
//

#ifndef ASSIGNMENT2_CAMERA_H
#define ASSIGNMENT2_CAMERA_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "../Main/initOpenGL/OpenGL_Deps.h"

//Include glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

class Camera {
private:
    vec3 cameraPosition = vec3(0.0f, 0.0f, 0.0f);
    vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
    vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
    //vec3 cameraRight = vec3(0.0f, 0.0f, 0.0f);
    //vec3 worldUp = vec3(0.0f, 1.0f, 0.0f);

    float cameraSpeed = 0.05f;
    float cameraFOV = 45.0f;

    GLfloat xRoll = -90.0f;
    GLfloat yRoll = 0.0f;

    struct Movement{
        bool forward;
        bool backward;
        bool right;
        bool left;
        bool up;
        bool down;
    };
    int window_width;
    int window_height;
    mat4 view;
    mat4 projection;
    //void updateCameraVectors();

public:
    Camera(int window_width, int window_height);
    //void centerView(int scaleX, int scaleY);
    //void fpsMouseMovement();
    //void moveCamera(bool forward, bool backward, bool right, bool left);
    void moveCamera(Movement movement, float deltaTime);
    void rotateView(vec2 mouseOffset);
    void changeCameraSpeed(float changeSpeed);

    void setupCameraTransformationMatrices(GLint viewLocation, GLint projectionLocation, GLint viewPosLoc);

    ~Camera()= default;

    mat4 getView();

    mat4 getProjection();
};


#endif //ASSIGNMENT2_CAMERA_H
