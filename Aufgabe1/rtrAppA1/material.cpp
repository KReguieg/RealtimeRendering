#include "material.h"

void PhongMaterial::setUniforms(QOpenGLShaderProgram &prog)
{
    prog.bind();
    prog.setUniformValue("material.k_ambient", k_ambient);
    prog.setUniformValue("material.k_diffuse", k_diffuse);
    prog.setUniformValue("material.k_specular", k_specular);
    prog.setUniformValue("material.shininess", shininess);
    prog.setUniformValue("ambientLightIntensity", ambientLightIntensity);
    prog.setUniformValue("light.position_EC", lightPos_EC);
    prog.setUniformValue("light.intensity", lightIntensity);
}
