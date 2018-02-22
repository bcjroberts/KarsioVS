#version 410
//Shader heavily based on https://learnopengl.com/'s PBR shader

out vec4 FragColor;

//in vec3 vertexColor; //this can be used for AO
in vec3 Normal;
in vec3 ModelPos;
in vec2 TexCoords;

struct Light{
    vec3 color;
    vec3 position;
}

struct TextureData{
    sampler2D albedo;
    sampler2D roughness;
    sampler2D metalness;
    sampler2D ambiantOcculusion;
};

// Texture samplers
uniform Light lights[100];
uniform vec3 viewPos;
uniform TextureData textureData;

//Globals
vec3 NORM = normalize(normal);
vec3 DIR_LIGHT = normalize(light.position - modelPos);
vec3 DIR_VEIW = normalize(viewPos - modelPos);

vec3 fresnel(){
    //rough estimate that works fine for most surfaces
    vec3 f0 = vec3(0.04); 

    //use albedo color if metalness is high enough
    f0 = mix(f0, textureData.albedo, textureData.metalness); 

    //compute fresnel using fresnel-schlick approximation
    return f0 + (1.0 - f0) * pow(1.0 - dot(NORM, DIR_VEIW), 5.0);
}

float geometry(float NdotV, float k){
    float denomator = NdotV * (1.0 - k) + k;

    return NdotV / denomator;
}

float geometry(){
    float NdotV = max(dot(NORM, DIR_VEIW), 0.0);
    float NdotL = max(dot(NORM, DIR_LIGHT), 0.0);
    float ggx1 = GeometryGGX(NdotV, k);
    float ggx2 = GeometryGGX(NdotL, k);
    
    return ggx1 * ggx2;
}

float distributionGGX(){
    float rSample = float(texture(textureData.roughness, uvCoord);
    float r2 = rSample * rSample;
    float NdotH = max(dot(NORM,HALF), 0.0);
    float NdotH_2 = NdotH * NdotH;
    float denomator = NdotH_2 * (r2 - 1.0) + 1.0;
    float denomator = PI * denomator * denomator;

    return r2 / denomator;
}

vec3 getSpecularLighting(){
    vec3 numerator = distributionGGX() * fresnel() * geometry();
    vec3 denomator = 4 * dot(DIR_VEIW, NORM) * dot(DIR_LIGHT, NORM);
    
    return numerator / denomator;
}


//Get the diffuse component of PBR lighting
vec3 getDiffuseLighting(){
    albedoSample = vec3(texture(textureData.albedo, uvCoord));
    return albedoSample / PI; 
}

void main() {
    vec3 finalColor = getDiffuseLighting() + getSpecularLighting();
    FragmentColour = vec4(finalColor, 1.0f);
}
