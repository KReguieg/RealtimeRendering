#version 150

/*
 * Simple Phong Shader
 * (C)opyright Hartmut Schirmacher, http://schirmacher.beuth-hochschule.de
 *
 * This vertex shader expects a position and normal vector per vertex,
 * and model-view, projection and normal matrices.
 *
 * It transforms the position and normal to eye coordinates and
 * passes them to the fragment shader; it also transforms the vertex 
 * position to clip coordinates for the needs of the pipeline.
 *
 */


// transformation matrices
uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;

// in: position and normal vector in model coordinates (_MC), texcoords
in vec3 position_MC;
in vec3 normal_MC;
in vec2 texcoord;

// position + normal vector in eye coordinates (_EC), texcoords
out vec4 position_EC;
out vec3 normal_EC;
out vec2 texcoord_frag;

void main(void) {
    
    // position to eye coordinates
    position_EC = modelViewMatrix * vec4(position_MC,1);

    // normal to eye coordinates
    normal_EC = normalMatrix * normal_MC;

    // just copy tex coords
    texcoord_frag = texcoord;

    // position to clip coordinates
    gl_Position = projectionMatrix * position_EC;

}
