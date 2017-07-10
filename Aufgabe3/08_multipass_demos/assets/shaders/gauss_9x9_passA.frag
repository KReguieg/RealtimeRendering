/*
 * Decomposed Linear Filter Shader 9x9, Pass A
 * (C)opyright Hartmut Schirmacher, http://schirmacher.beuth-hochschule.de
 *
 */

#version 150

// texture to be blurred
uniform sampler2D post_tex;

// size of the texture in pixels
uniform int image_width;

// use jittering?
uniform bool use_jitter;

// texture coords
in vec2 texcoord_frag;

// output: color
out vec4 outColor;


// simple pseudo random number
float rand(vec2 xy){
    return fract(sin(dot(xy, vec2(12.9898,78.233))) * 43758.5453);
}

// some constants
const int samples = 9;
const float center = (float(samples) - 1.0) / 2.0;
const float filterWeights[samples] = float[](0.0162162, 0.0540540, 0.1216216, 0.1945945, 0.2270270,
                                             0.1945945, 0.1216216, 0.0540540, 0.0162162 );

void main(void)
{
    float size  = float(samples) / float(image_width);
    vec3 color = vec3(0,0,0);
    for (int x = 0; x < samples; x++) {
        float jitter = use_jitter? 2*rand(texcoord_frag)-1 : 0;
        vec2 tc = texcoord_frag + vec2(x - center + jitter, 0) * size;
        color += texture(post_tex, tc).rgb * filterWeights[x];
    }
    outColor = vec4(color, 1);
}
