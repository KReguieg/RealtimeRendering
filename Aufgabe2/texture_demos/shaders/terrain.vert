/*
 *
 * vertex shader for phong + textures + bumps
 *
 */

#version 150

// transformation matrices
uniform mat4 modelViewProjectionMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

// in: position and normal vector in model coordinates (_MC)
in vec3 position_MC;
in vec3 normal_MC;
in vec3 tangent_MC;
in vec3 bitangent_MC;
in vec2 texcoord;

// point light
struct PointLight {
    vec3 intensity;
    vec4 position_EC;
};
uniform PointLight light;

struct DisplacementMaterial {
    float scale;
    sampler2D tex;
};
uniform DisplacementMaterial displacement;
uniform vec2 flyPosition;

struct Terrain{

    // additional textures
    sampler2D texture;
    sampler2D diffuseTexture;
    sampler2D temple;
    sampler2D temple_bump;
    sampler2D temple_displacement;
    float amplitude;


    // animation
    bool animateClouds;

};
uniform Terrain terrain;
// output - transformed to eye coordinates (EC)
out vec4 position_EC;
out vec3 normal_EC;

// output - transformed to tangent space (TS)
out vec3 viewDir_TS;
out vec3 lightDir_TS;

// tex coords - just copied
out vec2 texcoord_frag;


void main(void) {
    vec2 coord = texcoord+flyPosition;
    // displacement mapping!
    float displ = (1-texture(displacement.tex, coord * 2).r) * 0.04;
    vec4 pos = vec4(position_MC,1);

    float templePos = (1-texture(terrain.temple_displacement, coord * 2).r)*0.05;
    //if(displ * 40 >= 0.125)
    //    displ += templePos;
    pos += vec4(normal_MC,0)*templePos ;//* terrain.amplitude;

    if(templePos * 4 < 0.055){
        pos += vec4(normal_MC,0)* displ * terrain.amplitude * 0.5;
    }
    if(templePos* 4 > 0.055 && templePos* 4  <= 0.06){
        pos += vec4(normal_MC,0)* displ * terrain.amplitude * 0.2;
    }
    // vertex/fragment position in eye coordinates
    position_EC  = modelViewMatrix * pos;
    if(position_EC.y > 0 && position_EC.y * 50 >= 1)
        pos += vec4(normal_MC,0)*-50;
    // vertex/fragment position in clip coordinates
    gl_Position  = modelViewProjectionMatrix * pos;


    // normal in eye coordinates
    normal_EC = normalMatrix * normal_MC;

    // tex coords: just copy
    texcoord_frag = texcoord;

    // calculate position and T N B in world coordinates
    mat4 viewMatrixInverse = inverse(viewMatrix);
    vec4 wcPosition      = modelMatrix*vec4(position_MC,1.0);
    vec4 wcEyePosition   = viewMatrixInverse*vec4(0,0,0,1); // only works for perspective projection
    vec4 wcLightPosition = viewMatrixInverse*light.position_EC;
    vec3 wcNormal        = (modelMatrix*vec4(normal_MC, 0)).xyz;
    vec3 wcTangent       = (modelMatrix*vec4(tangent_MC, 0)).xyz;
    vec3 wcBitangent     = (modelMatrix*vec4(bitangent_MC, 0)).xyz;

    // light and view dir in WC
    vec3 wcLightDir = wcLightPosition.xyz - wcPosition.xyz;
    vec3 wcViewDir = wcEyePosition.xyz - wcPosition.xyz; // only for perspective!

    // now convert to TS
    mat3 TBN = mat3(wcTangent, wcBitangent, wcNormal);
    lightDir_TS = vec3(0,1,0);
    viewDir_TS  = wcViewDir * TBN;

}


