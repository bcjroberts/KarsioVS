#version 410
//Shader heavily based on https://learnopengl.com/code_viewer.php?code=in-practice/text_rendering&type=vertex

layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(vertex.xy, 0, 1.0);
    TexCoords = vertex.zw;
    // TexCoords = vec2(0);
} 