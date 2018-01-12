//
// Camera class heavily based on: https://learnopengl.com/code_viewer.php?type=header&code=camera
//

#include "Camera.h"

void Camera::setupCameraTransformationMatrices(GLint viewLocation, GLint projectionLocation, GLint viewPosLoc){
    // Create camera transformation
    view = lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
    //view = lookAt(vec3(0,0,5.85537815),vec3(0,0,4.85537815),vec3(0,1,0));
    projection = perspective(cameraFOV, (GLfloat)window_width/(GLfloat)window_height, 0.1f, 1000.0f);

    // Pass the matrices to the shader
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, value_ptr(view));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, value_ptr(projection));

    glUniform3f(viewPosLoc,cameraPosition.x,cameraPosition.y,cameraPosition.z);
}


void Camera::moveCamera(Movement movement, float deltaTime) {
    float velocity = (cameraSpeed) * (deltaTime);
    //cout<<velocity<<endl;
    if (movement.forward) {
        cameraPosition += cameraFront * velocity;
    }
    if (movement.backward) {
        cameraPosition -= cameraFront * velocity;
    }
    if (movement.right) {
        cameraPosition += normalize(cross(cameraFront, cameraUp)) * velocity;
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
}

void Camera::changeCameraSpeed(float changeSpeed) {
    float newCameraSpeed=cameraSpeed+changeSpeed;
    if(newCameraSpeed>0){
        cameraSpeed=newCameraSpeed;
    }
}

void Camera::rotateView(vec2 mouseOffset) {
    //cout<< xOffset<<":"<<yOffset<<endl;
    GLfloat sensitivity = 25;	// Change this value to your liking
    //mouseOffset.x *= sensitivity;
    //mouseOffset.y *= sensitivity;
    mouseOffset*=sensitivity;

    xRoll += mouseOffset.x;
    yRoll += mouseOffset.y;

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
}


mat4 Camera::getView(){
    return view;
}

mat4 Camera::getProjection(){
    return projection;
}

Camera::Camera(int window_width, int window_height) {
    this->window_width=window_width;
    this->window_height=window_height;
    cameraPosition = vec3(0.0,0.0,5.0);
    view = lookAt(vec3(0.0f, 0.0f, 3.0f),
                  vec3(0.0f,0.0f,0.0f),
                  vec3(0.0f, 0.0f, 0.0f));
}
