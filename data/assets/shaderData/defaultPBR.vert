#version 410
//Shader heavily based on: https://learnopengl.com/code_viewer.php?code=lighting/basic_lighting&type=vertex

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normalIn;
layout(location = 2) in vec2 textureCoord;

out vec3 Normal;
out vec3 ModelPos;
out vec2 TexCoords;

//uniform mat4 transformation;

uniform mat4 modelTransformation;
uniform mat4 view;
uniform mat4 projection;
// uniform mat3 tInverseModel;

void main() {
    //place the mesh in the scene
    gl_Position = projection * view * modelTransformation * vec4(position, 1.0);

    //Location of the mesh
    ModelPos = vec3(modelTransformation * (vec4(position, 1.0f)));

    //compute normal vectors
    Normal = mat3(modelTransformation) * normalIn;
    // Normal = mat3(transpose(inverse(modelTransformation)))*normalIn;
    TexCoords = vec2(textureCoord.x,1.0-textureCoord.y);
}
