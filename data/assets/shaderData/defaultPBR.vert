#version 410
//Shader heavily based on: https://learnopengl.com/code_viewer.php?code=lighting/basic_lighting&type=vertex

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTextureCoord;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBitangent;
layout(location = 5) in float isCrystal;

out VertexShader{
    vec3 modelPos;
    vec2 uvs;
    mat3 tangentLight;
    vec3 tangentView;
    vec3 tangentPos;
    vec3 viewPos;
    float isCrystal;
} vs_output;
// out vec3 Normal;
// out vec3 ModelPos;
// out vec2 TexCoords;

//uniform mat4 transformation;

uniform mat4 modelTransformation;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;
// uniform mat3 tInverseModel;

void main() {
    //Location of the mesh
    vs_output.modelPos = vec3(modelTransformation * (vec4(vPosition, 1.0f)));
    vs_output.uvs = vec2(vTextureCoord.x,1.0-vTextureCoord.y);

    mat3 normalMatrix = transpose(inverse(mat3(modelTransformation)));
    vec3 T = normalize(normalMatrix * vTangent);
    vec3 N = normalize(normalMatrix * vNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));
    vs_output.tangentLight = TBN;
    vs_output.tangentView = TBN * viewPos;
    vs_output.tangentPos = TBN * vs_output.modelPos;
    vs_output.viewPos = viewPos;

    vs_output.isCrystal = isCrystal;

    //place the mesh in the scene
    gl_Position = projection * view * modelTransformation * vec4(vPosition, 1.0);
}
