#include "material.h"
#include <assert.h>

#include <QOpenGLFunctions>

void SkyBoxMaterial::apply(unsigned int)
{
    prog_->bind();
    prog_->setUniformValue("cubeMap", tex_unit+0);
    prog_->setUniformValue("skybox.intensity_scale", intensity_scale);
    texture->bind(tex_unit+0);
}

void PostMaterial::apply(unsigned int)
{
    prog_->bind();

    // bind texture manually using its OpenGL ID
    QOpenGLFunctions gl(QOpenGLContext::currentContext());
    gl.glActiveTexture(GL_TEXTURE0 + tex_unit);
    gl.glBindTexture(GL_TEXTURE_2D, post_texture_id);
    prog_->setUniformValue("post_tex", tex_unit);
    prog_->setUniformValue("image_width", (GLint)image_size.width());
    prog_->setUniformValue("image_height", (GLint)image_size.height());
    prog_->setUniformValue("kernel_width", (GLint)kernel_size.width());
    prog_->setUniformValue("kernel_height", (GLint)kernel_size.height());
    prog_->setUniformValue("use_jitter", use_jitter);
}

void TexturedPhongMaterial::apply(unsigned int light_pass)
{
    prog_->bind();

    // globals
    prog_->setUniformValue("time", time);
    prog_->setUniformValue("ambientLightIntensity", ambientLightIntensity);

    // point light
    assert(light_pass>=0 && light_pass<lights.size());
    prog_->setUniformValue("light.position_WC", lights[light_pass].position_WC);
    prog_->setUniformValue("light.intensity", lights[light_pass].color * lights[light_pass].intensity);
    prog_->setUniformValue("light.pass", light_pass);

    // Phong
    prog_->setUniformValue("phong.k_ambient",  phong.k_ambient);
    prog_->setUniformValue("phong.k_diffuse",  phong.k_diffuse);
    prog_->setUniformValue("phong.k_specular", phong.k_specular);
    prog_->setUniformValue("phong.shininess",  phong.shininess);

    // Env Map parameters
    prog_->setUniformValue("envmap.useEnvironmentTexture", envmap.useEnvironmentTexture);
    prog_->setUniformValue("envmap.k_mirror",   envmap.k_mirror);
    prog_->setUniformValue("envmap.k_refract",  envmap.k_refract);
    prog_->setUniformValue("envmap.refract_ratio",  envmap.refract_ratio);

    // Planet textures
    prog_->setUniformValue("tex.useDiffuseTexture", tex.useDiffuseTexture);
    prog_->setUniformValue("tex.useEmissiveTexture", tex.useEmissiveTexture);
    prog_->setUniformValue("tex.useGlossTexture", tex.useGlossTexture);
    int unit = tex.tex_unit;
    if(envmap.useEnvironmentTexture) {
        prog_->setUniformValue("environmentTexture", unit);
        environmentTexture->bind(unit++);
    }
    if(tex.useDiffuseTexture) {
        prog_->setUniformValue("diffuseTexture", unit);
        diffuseTexture->bind(unit++);
    }
    if(tex.useEmissiveTexture) {
        prog_->setUniformValue("emissiveTexture", unit);
        emissiveTexture->bind(unit++);
    }
    if(tex.useGlossTexture) {
        prog_->setUniformValue("glossTexture", unit);
        glossTexture->bind(unit++);
    }
    prog_->setUniformValue("tex.emissive_scale", tex.emissive_scale);

    // bump & displacement mapping
    prog_->setUniformValue("bump.use", bump.use);
    if(bump.use) {
        prog_->setUniformValue("bump.scale", bump.scale);
        prog_->setUniformValue("bump.debug", bump.debug);
        prog_->setUniformValue("bumpTexture", unit); bump.tex->bind(unit++);

    }
    prog_->setUniformValue("displacement.use", displacement.use);
    if(displacement.use) {
        prog_->setUniformValue("displacement.scale", displacement.scale);
        prog_->setUniformValue("displacementTexture", unit); displacement.tex->bind(unit++);
    }


}



