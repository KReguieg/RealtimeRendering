/*
 * fragment shader for phong + textures + bumps
 *
 */

#version 150

// output - transformed to eye coordinates (EC)
in vec4 position_EC;
in vec3 normal_EC;

// output - transformed to tangent space (TS)
in vec3 viewDir_TS;
in vec3 lightDir_TS;

// tex coords - just copied
in vec2 texcoord_frag;

// output: color
out vec4 outColor;

// point light
struct PointLight {
    vec3 intensity;
    vec4 position_EC;
};
uniform PointLight light;

// Phong coefficients and exponent
struct PhongMaterial {

    // basic Phong
    vec3 k_ambient;
    vec3 k_diffuse;
    vec3 k_specular;
    float shininess;
    bool debug_texcoords;

};
uniform PhongMaterial phong;

struct Terrain{

    // additional textures
    sampler2D texture;
    sampler2D diffuseTexture;
    sampler2D glossTexture;
    sampler2D cloudsTexture;
    float night_scale;
    float night_blend_exp;


    // animation
    bool animateClouds;

};
uniform Terrain terrain;

struct BumpMaterial {
    bool debug;
    float scale;
    sampler2D tex;
};
uniform BumpMaterial bump;

struct DisplacementMaterial {
    float scale;
    sampler2D tex;
};
uniform DisplacementMaterial displacement;

uniform vec2 flyPosition;

vec3 decodeNormal(vec3 normal) {
    return normalize(normal * vec3(2, 2, 1) - vec3(1, 1, 0));
}

void main() {
    vec2 coords = texcoord_frag + flyPosition;
    vec3 bumpValue = texture(bump.tex, coords).xyz;
    vec3 n = decodeNormal(bumpValue);
    vec3 v = normalize(viewDir_TS);
    vec3 l = normalize(lightDir_TS);

    float ndotl = dot(n,l);
    float vdotl = dot(v,l);
    vec3 ambient = texture(terrain.texture, coords).rgb;
    vec3 diffuse = texture(terrain.diffuseTexture, coords).rgb * vdotl;
    vec3 displ = (1-texture(displacement.tex, coords).rgb) * 0.3;
    vec3 color = ambient + diffuse + displ;
    // set fragment color
    //outColor = vec4(texture(terrain.texture, texcoord_frag).rgb, 1.0) + vec4(texture(bump.tex, texcoord_frag).rgb, 1.0);
    //outColor = vec4(1);
    outColor = vec4(color, 1.0);
}
