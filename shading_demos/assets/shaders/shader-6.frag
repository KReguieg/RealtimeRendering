
// fragment shader that uses the normal direction as color

#version 150

in vec3 fragNormal;

out vec4 oColor;

void
main(void)
{
    oColor.rgb = (normalize(fragNormal) + vec3(1))/2;
    oColor.a = 1.0;
}

