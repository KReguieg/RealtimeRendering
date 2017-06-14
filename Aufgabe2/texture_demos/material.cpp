#include "material.h"


void WireframeMaterial::apply()
{
    prog_->bind();
    prog_->setUniformValue("wireframeColor", wireframeColor);
}

void VectorsMaterial::apply()
{
    prog_->bind();
    prog_->setUniformValue("vectorColor", vectorColor);
    prog_->setUniformValue("vectorToShow", vectorToShow);
    prog_->setUniformValue("scale", scale);

}

void PlanetMaterial::apply()
{
    prog_->bind();

    // globals
    prog_->setUniformValue("time", time);
    prog_->setUniformValue("ambientLightIntensity", ambientLightIntensity);

    // point light
    prog_->setUniformValue("light.position_EC", light.position_EC);
    prog_->setUniformValue("light.intensity", light.intensity);

    // Phong
    prog_->setUniformValue("phong.k_ambient", phong.k_ambient);
    prog_->setUniformValue("phong.k_diffuse", phong.k_diffuse);
    prog_->setUniformValue("phong.k_specular", phong.k_specular);
    prog_->setUniformValue("phong.shininess", phong.shininess);
    prog_->setUniformValue("phong.debug_texcoords", phong.debug_texcoords);

    // Planet textures
    prog_->setUniformValue("planet.useDayTexture", planet.useDayTexture);
    prog_->setUniformValue("planet.useNightTexture", planet.useNightTexture);
    prog_->setUniformValue("planet.useGlossTexture", planet.useGlossTexture);
    prog_->setUniformValue("planet.useCloudsTexture", planet.useCloudsTexture);
    if(planet.useDayTexture) {
        prog_->setUniformValue("planet.dayTexture", 0);
        planet.dayTexture->bind(0);
    }
    if(planet.useNightTexture) {
        prog_->setUniformValue("planet.nightTexture", 1);
        planet.nightTexture->bind(1);
    }
    if(planet.useGlossTexture) {
        prog_->setUniformValue("planet.glossTexture", 2);
        planet.glossTexture->bind(2);
    }
    if(planet.useCloudsTexture) {
        prog_->setUniformValue("planet.cloudsTexture", 3);
        planet.cloudsTexture->bind(3);
    }
    prog_->setUniformValue("planet.night_scale", planet.night_scale);
    prog_->setUniformValue("planet.night_blend_exp", planet.night_blend_exp);

    // Planet debugging & animation
    prog_->setUniformValue("planet.debug", planet.debug);
    prog_->setUniformValue("planet.debugWaterLand", planet.debugWaterLand);
    prog_->setUniformValue("planet.animateClouds", planet.animateClouds);

    // bump & displacement mapping
    prog_->setUniformValue("bump.use", bump.use);
    if(bump.use) {
        prog_->setUniformValue("bump.scale", bump.scale);
        prog_->setUniformValue("bump.tex", 4); bump.tex->bind(4);
        prog_->setUniformValue("bump.debug", bump.debug);
    }
    prog_->setUniformValue("displacement.use", displacement.use);
    if(displacement.use) {
        prog_->setUniformValue("displacement.scale", displacement.scale);
        prog_->setUniformValue("displacement.tex", 5); displacement.tex->bind(5);
    }
}

void TerrainMaterial::apply(){
    prog_->bind();

    // point light
    prog_->setUniformValue("light.position_EC", light.position_EC);
    prog_->setUniformValue("light.intensity", light.intensity);

    prog_->setUniformValue("bump.scale", bump.scale);
    prog_->setUniformValue("displacement.scale", displacement.scale);

    prog_->setUniformValue("terrain.texture", 0);
    terrain.texture->bind(0);
    prog_->setUniformValue("bump.tex", 1);
    bump.tex->bind(1);
    prog_->setUniformValue("terrain.diffuseTexture", 2);
    terrain.diffuseTexture->bind(2);
    prog_->setUniformValue("displacement.tex", 3);
    displacement.tex ->bind(3);

}



