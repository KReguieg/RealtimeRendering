#ifndef MATERIAL_H
#define MATERIAL_H

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include <memory>

/*
 *   Interface for surface materials.
 *
 */
class Material
{
public:

    // constructor requires an existing shader program
    Material(std::shared_ptr<QOpenGLShaderProgram> prog)
        :prog_(prog)
    {}

    // bind underlying shader program and set required uniforms
    virtual void apply(unsigned int light_pass = 0) = 0;

    // getter for the program object
    QOpenGLShaderProgram& program() const { return *prog_; }

protected:

    // reference to underlying shader program
    std::shared_ptr<QOpenGLShaderProgram> prog_;
};


class PostMaterial : public Material {
public:

    // constructor requires existing shader program
    PostMaterial(std::shared_ptr<QOpenGLShaderProgram> prog,
                 int texunit = 0) : Material(prog), tex_unit(texunit) {}

    // the texture to be post processed
    GLint post_texture_id;

    // the image size ("resolution") of the texture, needs to be set from outside
    QSize image_size;

    // kernel size of the filter to be applied
    QSize kernel_size = QSize(5,5);

    // use jittering of sample points?
    bool use_jitter = false;

    // texture unit to be used
    int tex_unit;

    // bind underlying shader program and set required uniforms
    void apply(unsigned int light_pass = 0) override;

};


class SkyBoxMaterial : public Material {
public:

    // constructor requires existing shader program
    SkyBoxMaterial(std::shared_ptr<QOpenGLShaderProgram> prog,
                    int texunit = 0) : Material(prog), tex_unit(texunit) {}

    // the cube map
    std::shared_ptr<QOpenGLTexture> texture;

    // intensity scaling factor
    float intensity_scale = 1.0;

    // texture unit to be used
    int tex_unit;

    // bind underlying shader program and set required uniforms
    void apply(unsigned int light_pass = 0) override;

};

class TexturedPhongMaterial : public Material {
public:

    // constructor requires existing shader program
    TexturedPhongMaterial(std::shared_ptr<QOpenGLShaderProgram> prog,
                          int texunit = 0) : Material(prog)
    {
        tex.tex_unit=texunit;
        lights.push_back(PointLight()); // make sure there is at least one light
    }

    // animation time
    float time = 0.0;

    // ambient light
    QVector3D ambientLightIntensity = QVector3D(0.3f,0.3f,0.3f);

    // properties of the Phong aspects of the material
    struct Phong {
        QVector3D k_ambient  = QVector3D(0.50f,0.10f,0.10f); // red-ish
        QVector3D k_diffuse  = QVector3D(0.50f,0.10f,0.10f); // red-ish
        QVector3D k_specular = QVector3D(0.80f,0.80f,0.80f); // white-ish
        float     shininess  = 80; // middle-ish
    } phong;

    struct EnvMap {
        bool useEnvironmentTexture = false;
        QVector3D k_mirror   = QVector3D(0.5f,0.5f,0.5f);
        QVector3D k_refract  = QVector3D(0,0,0);
        float refract_ratio = 1.5;
    } envmap;

    struct PointLight {
        QVector4D position_WC = QVector4D(0,1,5,1);
        QVector3D color = QVector3D(1,1,1);
        float intensity = 0.5;
    };
    std::vector<PointLight> lights;

    // planet-specific properties
    struct Textures {
        int tex_unit;     // first texture unit to be used
        bool useDiffuseTexture = false;
        bool useEmissiveTexture = false;
        bool useGlossTexture = false;
        float emissive_scale = 1.0;
    } tex;

    // textures cause trouble when inside structs :-(
    std::shared_ptr<QOpenGLTexture> diffuseTexture;
    std::shared_ptr<QOpenGLTexture> emissiveTexture;
    std::shared_ptr<QOpenGLTexture> glossTexture;
    std::shared_ptr<QOpenGLTexture> environmentTexture;

    // bump mapping
    struct Bump {
        bool use = false;
        float scale = 1.0;
        float debug = false;
        std::shared_ptr<QOpenGLTexture> tex;
    } bump;

    // displacement mapping
    struct Displacement {
        bool use = false;
        float scale = 1.0;
        std::shared_ptr<QOpenGLTexture> tex;
    } displacement;

    // bind underlying shader program and set required uniforms
    void apply(unsigned int light_pass = 0) override;

};


#endif // MATERIAL_H
