#version 410
//Render a single image to the screen

in vec2 TexCoords;

out vec4 FragmentColor;

uniform sampler2D image;

void main(){
    // FragmentColor = texture(image, TexCoords);
    FragmentColor = vec4(0.0f,0.0f,0.0f,1.0f);
    // FragmentColor = vec4(TexCoords.xy,0.5f,1.0f);
}