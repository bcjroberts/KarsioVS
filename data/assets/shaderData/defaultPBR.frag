#version 410
//Shader heavily based on https://learnopengl.com/'s PBR shader

out vec4 FragColor;

//in vec3 vertexColor; //this can be used for AO
in vec3 Normal;
in vec3 ModelPos;
in vec2 TexCoords;

struct Light{
    vec3 position;
    vec3 color;
};

struct Textures{
    sampler2D albedo;
    sampler2D roughness;
    sampler2D metalness;
	sampler2D normal;
    sampler2D emission;
    // sampler2D ambiantOcculusion;
};

const int numLights = 10;

uniform Light lights[numLights];
uniform vec3 viewPos;
uniform Textures textureData;

// float distributionGGX(vec3 N, vec3 H, float roughness);
// float geometrySchlickGGX(float NdotV, float roughness) ;
// float geometrySmith(vec3 norm, vec3 viewDir, vec3 lightDir, float roughness) ;
// vec3 fresnelSchlick(float cosTheta, vec3 F0) ;
// vec3 calculateReflectance(int i, vec3 norm, vec3 viewDir, float roughness);
// vec3 computeColor();

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
float distributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    // float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;
}
// ----------------------------------------------------------------------------
float geometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    // float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return NdotV / denom;
}
// ----------------------------------------------------------------------------
float geometrySmith(vec3 norm, vec3 viewDir, vec3 lightDir, float roughness) {
    float NdotV = max(dot(norm, viewDir), 0.0);
    float NdotL = max(dot(norm, lightDir), 0.0);
    float ggx1 = geometrySchlickGGX(NdotV, roughness);
    float ggx2 = geometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 calculateReflectance(int i, vec3 norm, vec3 viewDir, vec3 albedo, float roughness, float metalness, vec3 fresnel0){
    vec3 lightMagnitude = lights[i].position - ModelPos;

    // calculate per-light radiance
    vec3 lightDir = normalize(lightMagnitude);
    vec3 halfNorm = normalize(viewDir + lightDir);
    float lightDis = length(lightMagnitude);
    float attenuation = 1.0 / (lightDis * lightDis);
    vec3 radiance = lights[i].color * attenuation;

    // Cook-Torrance BRDF
    float compDistribution = distributionGGX(norm, halfNorm, roughness);   
    float compGeometry = geometrySmith(norm, viewDir, lightDir, roughness);      
    vec3 compFresnel = fresnelSchlick(max(dot(halfNorm, viewDir), 0.0), fresnel0);
        
    // compute specular component
    vec3 nominator = compDistribution * compGeometry * compFresnel; 
    // 0.001 to prevent divide by zero.
    float denominator = 4 * max(dot(norm, viewDir), 0.0) * max(dot(norm, lightDir), 0.0) + 0.001; 
    vec3 specular = nominator / denominator;
    
    // kS is equal to Fresnel
    // vec3 kSpecular = compFresnel;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    vec3 kDiffuse = (vec3(1.0) - compFresnel) * (1.0 - metalness);
    // kDiffuse *= 1.0 - matalness;	  

    // scale light by NdotL
    float NdotL = max(dot(norm, lightDir), 0.0);        

    return (kDiffuse * albedo / PI + specular) * radiance * NdotL;
}

vec3 computeColor(){
    vec3 albedo = pow(texture(textureData.albedo, TexCoords).rgb, vec3(2.2));
    float metalness = texture(textureData.metalness, TexCoords).r;
    float roughness = texture(textureData.roughness, TexCoords).r;
    // float ao = texture(aoMap, TexCoords).r;

    // vec3 albedo = vec3(0.9,0.9,0.9);
    // float metalness = 0;
    // float roughness = 0.2;

    // vec3 N = getNormalFromMap();
    vec3 norm = normalize(Normal);

    vec3 viewDir = normalize(viewPos - ModelPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 fresnel0 = vec3(0.04); 
    fresnel0 = mix(fresnel0, albedo, metalness);

    // reflectance equation
    vec3 reflectance = vec3(0.0);
    for(int i = 0; i < numLights; ++i) {
        // add to outgoing radiance to fragment reflectance
        reflectance += calculateReflectance(i, norm, viewDir, albedo, roughness, metalness, fresnel0);  
        // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    // vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 ambient = vec3(0.03) * albedo;    
    
    // vec3 color = ambient + reflectance;
    return ambient + reflectance;
}

// ----------------------------------------------------------------------------
void main() {
    // vec3 emission = vec3(0);
    vec3 emission = pow(texture(textureData.emission, TexCoords).rgb, vec3(2.2));
    vec3 color = max(computeColor(),emission);
    
    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}