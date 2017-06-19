#version 150

uniform samplerCube cubeMap;

// matrices provided by the camera
uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float flyHeight;
uniform vec3 lookDirection;

// vertex position and normal from vertex shader, in eye coordinates
in vec4 position_EC;
in vec3 normal_EC;
in vec2 texcoord_frag;

out vec4 outColor;

void main(void)
{
    vec3 toVertexEC = normalize(position_EC.xyz );
    vec3 mvp = (modelViewProjectionMatrix * vec4(toVertexEC, 0)).xyz;
    vec3 toVertexWC = (inverse(viewMatrix) * vec4(toVertexEC, 0)).xyz;
    float ndotl =  (dot(toVertexWC, normal_EC)) * 20 ;
    vec3 sky = texture(cubeMap, toVertexWC ).rgb;
    outColor = vec4(sky, 1);

    float f = (toVertexWC.y + flyHeight) * 5;
    if(f <= 0.5 && f >= -0.2)
        outColor -= vec4(sin(0.5-f));

}
