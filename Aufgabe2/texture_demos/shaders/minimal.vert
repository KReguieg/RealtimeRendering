#version 150

/*
 * Minimal Vertex Shader - only applies model-view-projection matrix
 * (C)opyright Hartmut Schirmacher, http://schirmacher.beuth-hochschule.de
 *
 */


// transformation matrices as provided by Cinder
uniform mat4 modelViewProjectionMatrix;

// in: position and normal vector in model coordinates (_MC)
in vec3 position_MC;

void main(void) {
    
    // position to clip coordinates
    gl_Position = modelViewProjectionMatrix * vec4(position_MC,1);
        
}
