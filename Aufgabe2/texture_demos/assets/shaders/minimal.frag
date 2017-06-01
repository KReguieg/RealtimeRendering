#version 150

/*
 * Minimal Phong Shader - applies a fixed color to every rendered fragment
 * (C)opyright Hartmut Schirmacher, http://schirmacher.beuth-hochschule.de
 *
 */


out vec4 color;      // final color of fragment

void main(void)
{
    color = vec4(1,0,0,1); // red
    
}
