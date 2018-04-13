#version 410
//Shader heavily based on: https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.2.2.point_shadows_soft/3.2.2.point_shadows_depth.vs

layout (location = 0) in vec3 vPosition;

uniform mat4 modelTransformation;

void main()
{
    gl_Position = modelTransformation * vec4(vPosition, 1.0);
}