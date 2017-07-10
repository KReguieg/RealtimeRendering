/*
 * Post processing shader just rendering what is in a texture
 * (C)opyright Hartmut Schirmacher, http://schirmacher.beuth-hochschule.de
 *
 */

#version 150

// texture to be blurred
uniform sampler2D post_tex;

// texture coords
in vec2 texcoord_frag;

// output: color
out vec4 outColor;

void main(void)
{
    // texture lookup
    vec3 color = texture(post_tex, texcoord_frag).rgb;
    
    // out to frame buffer
    outColor = vec4(color, 1);
    // outColor = vec4(texcoord_frag.s, texcoord_frag.t, 0, 1);
}
