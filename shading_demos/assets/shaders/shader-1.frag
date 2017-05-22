
// "minimal" fragment shader

#version 150

out vec4 oColor;

void
main(void)
{
    oColor.rgb = vec3(0.7, 0.3, 0.3);
    oColor.a   = 1.0;
}

