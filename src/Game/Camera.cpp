//
// Camera class heavily based on: https://learnopengl.com/code_viewer.php?type=header&code=camera
//

#include "Camera.h"
#include <glm/gtx/vector_angle.hpp>

using namespace glm;

void Camera::moveCamera(Movement movement, float deltaTime) {
    float velocity = (cameraSpeed) * (deltaTime);
    //std::cout<<"velocity: " << velocity<<std::endl;
    if (movement.forward) {
        cameraPosition += cameraFront * velocity;
    }
    if (movement.backward) {
        cameraPosition -= cameraFront * velocity;
    }
    if (movement.right) {
        cameraPosition += normalize(cross(cameraFront, cameraUp)) * velocity;
        //cameraPosition += glm::vec3(0, 0, deltaTime);
        //cameraPosition += normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (movement.left) {
        cameraPosition -= normalize(cross(cameraFront, cameraUp)) * velocity;
        //cameraPosition -= normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if(movement.up){
        cameraPosition += cameraUp * velocity;
    }
    if(movement.down){
        cameraPosition -= cameraUp * velocity;
    }
    lookAtPos = cameraPosition + cameraFront;
    //printf("new camera pos: (%f,%f,%f)\n", cameraPosition.x, cameraPosition.y, cameraPosition.z);
}



void Camera::changeCameraSpeed(float changeSpeed) {
    float newCameraSpeed=cameraSpeed+changeSpeed;
    if(newCameraSpeed>0){
        cameraSpeed=newCameraSpeed;
    }
}

void Camera::rotateView(vec2 mouseOffset) {
    //cout<< xOffset<<":"<<yOffset<<endl;
    GLfloat sensitivity = 100;	// Change this value to your liking
    mouseOffset*=sensitivity;

    xRoll = mouseOffset.x;
    yRoll = mouseOffset.y;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (yRoll > 89.0f){
        yRoll = 89.0f;
    }
    if (yRoll < -89.0f){
        yRoll = -89.0f;
    }
    vec3 front = vec3(cos(radians(xRoll)) * cos(radians(yRoll)),
                      sin(radians(yRoll)),
                      sin(radians(xRoll)) * cos(radians(yRoll)));
    cameraFront = normalize(front);
    lookAtPos = cameraPosition + cameraFront;
	//std::cout << xRoll << " : " << yRoll << std::endl;
}

void Camera::rotateCameraTowardPoint(glm::vec3 point, float amount) {
    lookAtPos = glm::mix(lookAtPos, point, amount);
}

void Camera::lerpCameraTowardPoint(glm::vec3 point, float amount) {
	cameraPosition = glm::mix(cameraPosition, point, amount);
}


mat4 Camera::getView(){
	view = lookAt(cameraPosition, lookAtPos, cameraUp);
    return view;
}

mat4 Camera::getProjection(){
	projection = perspective(cameraFOV, (GLfloat)*window_width / (GLfloat)*window_height, 0.1f, 1000.0f);
    return projection;
}

vec3 Camera::getPosition() {
	return cameraPosition;
}

vec3 Camera::getForward() {
    return lookAtPos;
}

vec3 Camera::getUp() {
    return cameraUp;
}

Camera::Camera(int *window_width, int *window_height) {
    this->window_width=window_width;
    this->window_height=window_height;
    cameraPosition = vec3(-15.0,5,5.0);
	cameraFront = vec3(0.0f, 0.0f, -1.0f);
	cameraUp = vec3(0.0f, 1.0f, 0.0f);
	view = lookAt(cameraPosition, lookAtPos, cameraUp);
	cameraSpeed = 0.05;
	cameraFOV = 45.0;
	xRoll = -90;
	yRoll = 0;
//    view = lookAt(vec3(0.0, 4.0, 20.0),
//                  vec3(0.0f,0.0f,0.0f),
//                  vec3(0.0f, 0.0f, 0.0f));
	projection = perspective(cameraFOV, (GLfloat)*window_width / (GLfloat)*window_height, 0.1f, 1000.0f);
}
