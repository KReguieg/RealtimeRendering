
// fragment shader that uses the normal direction as color

#version 150

in vec2 fragTexCoord;

out vec4 oColor;

void
main(void)
{
    vec3 color = vec3(0.8,0.2,0);

    oColor.rgb = mod(fragTexCoord.x, 0.1) < 0.05 ?
                   color : vec3(1,1,1)-color;
    oColor.a = 1;
}

