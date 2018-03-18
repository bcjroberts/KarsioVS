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

//using namespace glm;

struct Movement
{
    bool forward;
    bool backward;
    bool right;
    bool left;
    bool up;
    bool down;
};

class Camera {
private:
	glm::vec3 cameraPosition;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
    //vec3 cameraRight = vec3(0.0f, 0.0f, 0.0f);
    //vec3 worldUp = vec3(0.0f, 1.0f, 0.0f);

    float cameraSpeed;
    float cameraFOV;

    GLfloat xRoll;
	GLfloat yRoll;

    int *window_width;
    int *window_height;
    glm::mat4 view;
    glm::mat4 projection;
    //void updateCameraVectors();

public:
    glm::vec3 lookAtPos;
    Camera(int *window_width, int *window_height);
    //void centerView(int scaleX, int scaleY);
    //void fpsMouseMovement();
    //void moveCamera(bool forward, bool backward, bool right, bool left);
    void moveCamera(Movement movement, float deltaTime);
    void rotateView(glm::vec2 mouseOffset);
	void rotateCameraTowardPoint(glm::vec3 point, float amount);
	void lerpCameraTowardPoint(glm::vec3 point, float amount);
    void changeCameraSpeed(float changeSpeed);

    void setupCameraTransformationMatrices(GLint viewLocation, GLint projectionLocation, GLint viewPosLoc);

    ~Camera()= default;

    glm::mat4 getView();
    glm::mat4 getProjection();
	glm::vec3 getPosition();
};


#endif //ASSIGNMENT2_CAMERA_H
