/*
 * Depth Of Field Shader
 * (C)opyright Christopher Hahn & Ibrahim Khaled Reguieg, https://github.com/KReguieg/RealtimeRendering
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
float normpdf(float x, float sigma)
{
        return 0.39894*exp(-0.5*x*x / (sigma*sigma)) / sigma;
}


void main(void)
{
    vec2 image_size = vec2(image_width, image_height);
    vec2 kernel_size = vec2(kernel_width, kernel_height);
    vec2 size = kernel_size / image_size;
    vec2 center = (kernel_size - vec2(1.0,1.0)) / 2.0;
    float z = texture(post_tex, texcoord_frag.xy).a;

    vec4 color = vec4(0);
    float kernel[20];
    int kSize = int(kernel_width - 1) / 2;

    for (int j = 0; j <= kSize; ++j)
    {
        kernel[kSize + j] = kernel[kSize - j] = normpdf(float(j), 1);
    }


    float sum = 0;
    float faktor;

    for (int x = 0; x < kernel_width; x++) {
        for (int y = 0; y < kernel_height; y++) {
            vec2 jitter = use_jitter? rand2(texcoord_frag) * 2.0 - 1.0 : vec2(0,0);
            float value = normpdf(jitter.x,1) * normpdf(jitter.y,1);
            vec2 coord = texcoord_frag + (vec2(x, y) - center + jitter) * size * z;
            vec4 tempcolor = texture(post_tex, coord).rgba;

            float diff = z - tempcolor.a;
            if(diff < 0.01) {
                faktor =  z;
            } else {
                faktor =  tempcolor.a ;
            }
            faktor = value;
            color += tempcolor * faktor ;
            sum += faktor;
        }
    }

    color /= sum;

    //vec4 finalcolor = vec4(0);

    // finalcolor = (1 - z) * color + z * texture(post_tex, texcoord_frag.xy);
    //if(z >= 0.99)
    //    finalcolor = color;
    outColor = color;
}

