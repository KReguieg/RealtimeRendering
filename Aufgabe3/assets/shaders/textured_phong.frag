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

in float z;

// output: color
out vec4 outColor;

struct PointLight {
    vec3 intensity;
    vec4 position_WC;
    int  pass;
};

struct PhongMaterial {
    vec3 k_ambient;
    vec3 k_diffuse;
    vec3 k_specular;
    float shininess;
    bool debug_texcoords;

};

struct EnvMap {
    bool useEnvironmentTexture;
    vec3 k_mirror;
    vec3 k_refract;
    float refract_ratio;
};

struct TexturedMaterial {
    bool useDiffuseTexture;
    bool useEmissiveTexture;
    bool useGlossTexture;
    float emissive_scale;
};

struct BumpMaterial {
    bool use;
    bool debug;
    float scale;
};

struct DisplacementMaterial {
    bool use;
    float scale;
};

uniform PointLight light;
uniform PhongMaterial phong;
uniform TexturedMaterial tex;
uniform BumpMaterial bump;
uniform DisplacementMaterial displacement;
uniform EnvMap envmap;
uniform samplerCube environmentTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D emissiveTexture;
uniform sampler2D glossTexture;
uniform sampler2D bumpTexture;
uniform sampler2D displacementTexture;

// standard uniforms
uniform mat4  viewMatrix;
uniform mat4  projectionMatrix;
uniform float time;
uniform vec3  ambientLightIntensity;

/*
 *  Calculate surface color based on Phong illumination model.
 */

vec3 texphong(vec3 n, vec3 v, vec3 l, vec2 uv) {

    // texture lookups
    vec3  diffCol  = texture(diffuseTexture, uv).rgb;
    vec3  emissCol = texture(emissiveTexture, uv).rgb * tex.emissive_scale;
    float gloss    = texture(glossTexture, uv).r * 255.0; // 0...255

    // cosine of angle between light and surface normal.
    float ndotl = dot(n,l);

    // ambient / emissive part
    vec3 ambient = vec3(0,0,0);
    if(light.pass == 0) // only add ambient in first light pass
        ambient = tex.useEmissiveTexture?
                  emissCol : phong.k_ambient * ambientLightIntensity;

    // surface back-facing to light?
    if(ndotl<=0.0)
        return ambient;
    else
        ndotl = max(ndotl, 0.0);

    // diffuse contribution
    vec3 diffuseCoeff = tex.useDiffuseTexture? diffCol : phong.k_diffuse;

    // final diffuse term for daytime
    vec3 diffuse =  diffuseCoeff * light.intensity * ndotl;

    // reflected light direction = perfect reflection direction
    vec3 r = reflect(-l,n);

    // cosine of angle between reflection dir and viewing dir
    float rdotv = max( dot(r,v), 0.0);

    // specular contribution + gloss map
    float shininess = tex.useGlossTexture? gloss : phong.shininess;
    vec3 specular = phong.k_specular * light.intensity * pow(rdotv, shininess);

    // return sum of all contributions
    return ambient + diffuse + specular;

}

vec3 decodeNormal(vec3 normal) {
    return normalize(normal * vec3(2, 2, 1) - vec3(1, 1, 0));
}

void main() {

    // default normal in tangent space is (0,0,1).
    vec3 bumpValue = texture(bumpTexture, texcoord_frag).xyz;

    // get bump direction (in tangent space) from bump texture
    vec3 N = bump.use? decodeNormal(bumpValue) : vec3(0,0,1);
    vec3 V = normalize(viewDir_TS);
    vec3 L = normalize(lightDir_TS);

    // calculate color using phong illumination
    vec3 final_color = texphong(N, V, L, texcoord_frag);

    // calculate reflection of environment
    vec3 normalEC = normalize(normal_EC);
    vec3 viewdirEC = normalize(-position_EC.xyz);
    vec3 reflEC = reflect(-viewdirEC, normalEC); // note: not from bump map!
    vec3 reflWC = (inverse(viewMatrix) * vec4(reflEC,0.0)).xyz;
    vec3 c_mirror = envmap.k_mirror * texture(environmentTexture, reflWC).rgb;

    vec3 refrEC = refract(-viewdirEC, normalEC, envmap.refract_ratio);
    vec3 refrWC = (inverse(viewMatrix) * vec4(refrEC,0.0)).xyz;
    vec3 c_refract = envmap.k_refract * texture(environmentTexture, refrWC).rgb;

    if(envmap.useEnvironmentTexture)
        final_color += c_mirror + c_refract;

    outColor = vec4(final_color, 1-z/10);

}
