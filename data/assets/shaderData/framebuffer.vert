#version 410
//Render a single image to the screen

layout(location = 0) in vec4 vertex;
out vec2 TexCoords;

//uniform mat4 transformation;

uniform mat4 projection;
uniform mat4 transformation;

void main() {
    //Get position for where the plane should be drawn
    gl_Position = projection * transformation * vec4(vertex.xy, 0.0, 1.0);
    // gl_Position = vec4(vertex.xy, 0.0, 1.0);
    // gl_Position = vec4(0, 0, 0.0, 1.0);

    //Get UV coordinates
    TexCoords = vec2(vertex.z,1-vertex.w);
    // TexCoords = vec2(0,0);
}
