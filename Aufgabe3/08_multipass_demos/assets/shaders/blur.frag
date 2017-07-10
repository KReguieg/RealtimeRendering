/*
 * Simple Blur Shader
 * (C)opyright Hartmut Schirmacher, http://schirmacher.beuth-hochschule.de
 *
 */

#version 150

// texture to be blurred
uniform sampler2D post_tex;
uniform int image_width;
uniform int image_height;
uniform int kernel_width;
uniform int kernel_height;

// use jittering?
uniform bool use_jitter;

// texture coords
in vec2 texcoord_frag;

// output: color
out vec4 outColor;

// simple pseudo random number
float rand(vec2 xy)
{
    return fract(sin(dot(xy, vec2(12.9898,78.233))) * 43758.5453);
}
vec2 rand2(vec2 xy)
{
    return vec2(rand(xy.xy), rand(xy.yx));
}

void main(void)
{
    vec2 image_size = vec2(image_width, image_height);
    vec2 kernel_size = vec2(kernel_width, kernel_height);
    vec2 size = kernel_size / image_size;
    vec2 center = (kernel_size - vec2(1.0,1.0)) / 2.0;

    vec3 color = vec3(0,0,0);
    for (int x = 0; x < kernel_width; x++) {
        for (int y = 0; y < kernel_height; y++) {
            vec2 jitter = use_jitter? rand2(texcoord_frag) * 2.0 - 1.0 : vec2(0,0);
            vec2 coord = texcoord_frag + (vec2(x, y) - center + jitter) * size;
            color += texture(post_tex, coord).rgb;
        }
    }
    color *= 1.0/float(kernel_size.x*kernel_size.y);
    
    // out to frame buffer
    outColor = vec4(color, 1);
    // outColor = vec4(texcoord_frag.s, texcoord_frag.t, 0, 1);

}
