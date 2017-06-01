/* 
 *  geom shader examples by Hartmut Schirmacher
 */

#version 150

uniform vec3 wireframeColor;

out vec4 outColor;

void
main(void)
{
    outColor = vec4(wireframeColor, 1);
}
