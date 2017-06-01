/*
 * Fragment Phong Shader extended to be a "Planet Earth" shader
 * Author: Hartmut Schirmacher
 *
 * expects position and normal vectors in eye coordinates per vertex;
 * expects uniforms for ambient light, directional light, and phong material.
 * 
 *
 */

#version 150

// position and normal in eye coordinates
in vec4  position_EC;
in vec3  normal_EC;
in vec2  texcoord_frag;

// output: color
out vec4 outColor;

// point light
struct PointLight {
    vec3 intensity;
    vec4 position_EC;
};

// Phong coefficients and exponent
struct PhongMaterial {

    // basic Phong
    vec3 k_ambient;
    vec3 k_diffuse;
    vec3 k_specular;
    float shininess;

};

struct PlanetMaterial {

    // additional textures
    bool useDayTexture;
    bool useNightTexture;
    bool useGlossTexture;
    bool useCloudsTexture;
    sampler2D dayTexture;
    sampler2D nightTexture;
    sampler2D glossTexture;
    sampler2D cloudsTexture;

    // debugging
    bool debug;
    bool debugWaterLand;

    // animation
    bool animateClouds;

};


struct BumpMaterial {
    float scale;
};

struct DisplacementMaterial {
    float scale;
};

// uniforms
uniform mat4  projectionMatrix;
uniform float time;

uniform vec3 ambientLightIntensity;
uniform PointLight light;
uniform PhongMaterial phong;
uniform PlanetMaterial planet;
uniform BumpMaterial bump;
uniform DisplacementMaterial displacement;

/*
 *  Calculate surface color based on Phong illumination model.
 */

vec3 planetshader(vec3 n, vec3 v, vec3 l, vec2 uv) {
    
    // animation
    vec2 uv_clouds = planet.animateClouds? texcoord_frag+vec2(time*0.02,0) : texcoord_frag;
    
    // texture lookups
    vec3  dayCol = texture(planet.dayTexture, uv).rgb;
    vec3  nightCol = texture(planet.nightTexture, uv).rgb;
    bool  atSea = texture(planet.glossTexture, uv).r > 0.008;
    float cloudDensity = texture(planet.cloudsTexture, uv_clouds).r;
    
    // make brighter / gamma correction
    dayCol = pow(dayCol, vec3(0.6))*2.0;
    nightCol = pow(nightCol, vec3(0.5))*1.5;
    cloudDensity = pow(cloudDensity, 0.8);

    // cosine of angle between light and surface normal.
    float ndotl = dot(n,-l);

    // debugging factor
    float debugFactor = 1.0;

    if(planet.debug) {

        // visualize s texture coordinates using stripes
        if(mod(uv.x, 0.05) >= 0.025) {
            debugFactor = 0.7;
        }
        // visualize day-night threshold via cos of threshold angle 
        float thresh = cos(87.0*3.1415/180.0);
        if(ndotl>0.0 && ndotl<thresh) {
            return vec3(0.2,0.6,0.2);
        }
    }

    // ambient part without clouds
    vec3 ambient = (planet.useNightTexture? nightCol : phong.k_ambient) *
                   ambientLightIntensity * debugFactor;
    // just show water vs. land
    if(planet.debugWaterLand) {
        ambient = atSea? vec3(0.5,0,0) : vec3(0,0.5,0);
    }
    // clouds at night?
    if(planet.useCloudsTexture) {
        ambient = (1.0-cloudDensity)*ambient + cloudDensity * vec3(0.1,0.1,0.1);
    }

    // surface back-facing to light?
    if(ndotl<=0.0)
        return ambient;
    else
        ndotl = max(ndotl, 0.0);

    // diffuse contribution
    vec3 diffuseCoeff = planet.useDayTexture? dayCol : phong.k_diffuse;
    if(planet.debugWaterLand) {
        diffuseCoeff = atSea? vec3(0.8,0,0) : vec3(0,0.8,0);
    }
    // clouds at day?
    if(planet.useCloudsTexture) {
        diffuseCoeff = (1.0-cloudDensity)*diffuseCoeff + cloudDensity*vec3(1.5,1.5,1.5);
    }
    // final diffuse term for daytime
    vec3 diffuse =  diffuseCoeff * light.intensity * ndotl * debugFactor;

    // ambient part contains lights; modify depending on time of day
    ambient *= pow(1.0-ndotl,1.0);
    
     // reflected light direction = perfect reflection direction
    vec3 r = reflect(l,n);
    
    // cosine of angle between reflection dir and viewing dir
    float rdotv = max( dot(r,v), 0.0);
    
    // specular contribution
    bool loShine = planet.useGlossTexture && !atSea;
    vec3 specularCoeff = phong.k_specular;
    float exponent = phong.shininess;
    // gloss map
    if(loShine) {
        specularCoeff /= 6;
        exponent = 15;
    }
    vec3 specular = specularCoeff * light.intensity * pow(rdotv, exponent);
 
    // return sum of all contributions
    return ambient + diffuse + specular;
    
}



void main() {
    
    // normalize normal after projection
    vec3 normalEC = normalize(normal_EC);
    
    // do we use a perspective or an orthogonal projection matrix?
    bool usePerspective = projectionMatrix[2][3] != 0.0;
    
    // calculate light direction (for point light)
    vec3 lightDirEC = normalize(position_EC - light.position_EC).xyz;
    
    // for perspective mode, the viewing direction (in eye coords) points
    // from the vertex to the origin (0,0,0) --> use -ecPosition as direction.
    // for orthogonal mode, the viewing direction is simply (0,0,1)
    vec3 viewdirEC = usePerspective? normalize(-position_EC.xyz) : vec3(0,0,1);
    
    // calculate color using phong illumination
    vec3 color = planetshader(normalEC, viewdirEC, lightDirEC, texcoord_frag);
    
    // set fragment color
    outColor = vec4(color, 1.0);
    
}
