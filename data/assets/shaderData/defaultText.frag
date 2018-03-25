#version 410
//Shader heavily based on:
// https://learnopengl.com/code_viewer.php?code=in-practice/text_rendering&type=fragment

in vec2 TexCoords;
out vec4 FragmentColor;

uniform sampler2D text;
uniform vec3 textColor;

void main() {    
    FragmentColor = vec4(textColor.xyz, texture(text,TexCoords).r);
    // FragmentColor = vec4(1.0f,0,0,1.0f);
}