
// fragment shader that sets a uniform color

#version 150

uniform vec3 myUniformColor;

out vec4 oColor;

void
main(void)
{
    oColor = vec4(myUniformColor,1);
}

