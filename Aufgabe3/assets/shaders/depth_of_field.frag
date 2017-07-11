/*
 * Depth Of Field Shader
 * (C)opyright Christopher Hahn & Ibrahim Khaled Reguieg, https://github.com/KReguieg/RealtimeRendering
 *
 */

#version 150

// texture to be blurred
uniform sampler2D post_tex;
uniform vec3 focusPoint;

// texture coords
in vec2 texcoord_frag;

// output: color
out vec4 outColor;

void main(void)
{
    outColor = texture(post_tex, texcoord_frag.xy);

}
