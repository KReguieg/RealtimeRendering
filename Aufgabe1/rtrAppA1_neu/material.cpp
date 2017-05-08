#include "material.h"

void PhongMaterial::apply()
{
    prog_->bind();
    prog_->setUniformValue("toon.shadingLevels", shadingLevels);
    prog_->setUniformValue("material.k_ambient", k_ambient);
    prog_->setUniformValue("material.k_diffuse", k_diffuse);
    prog_->setUniformValue("material.k_specular", k_specular);
    prog_->setUniformValue("material.shininess", shininess);
    prog_->setUniformValue("ambientLightIntensity", ambientLightIntensity);
    prog_->setUniformValue("light.position_EC", lightPos_EC);
    prog_->setUniformValue("light.intensity", lightIntensity);
}
