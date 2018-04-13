#version 410
//Shader heavily based on:
//https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.2.2.point_shadows_soft/3.2.2.point_shadows_depth.fs

in vec4 FragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
    float lightDistance = length(FragPos.xyz - lightPos);
    
    // map to [0;1] range by dividing by farPlane
    lightDistance = lightDistance / farPlane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
}