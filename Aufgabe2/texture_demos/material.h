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
    virtual void apply() = 0;

    // getter for the program object
    QOpenGLShaderProgram& program() const { return *prog_; }

protected:

    // reference to underlying shader program
    std::shared_ptr<QOpenGLShaderProgram> prog_;
};

class WireframeMaterial : public Material {
public:

    // constructor requires existing shader program
    WireframeMaterial(std::shared_ptr<QOpenGLShaderProgram> prog) : Material(prog) {}

    QVector3D wireframeColor = QVector3D(1,1,0);

    // bind underlying shader program and set required uniforms
    void apply() override;

};

// material to visualize normal, tangent, bitangent
class VectorsMaterial : public Material {
public:

    // constructor requires existing shader program
    VectorsMaterial(std::shared_ptr<QOpenGLShaderProgram> prog) : Material(prog) {}


    int vectorToShow = 1; // 1=normal, 2=tangent, 3=bitangent
    QVector3D vectorColor = QVector3D(1,1,0);
    float scale = true;

    // bind underlying shader program and set required uniforms
    void apply() override;

};


class PlanetMaterial : public Material {
public:

    // constructor requires existing shader program
    PlanetMaterial(std::shared_ptr<QOpenGLShaderProgram> prog) : Material(prog) {}

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
        bool debug_texcoords = false;
    } phong;

    // light position: up right from the camera, in eye coordinates
    struct PointLight {
        QVector4D position_EC = QVector4D(2,2,0,1);
        QVector3D intensity = QVector3D(1,1,1);
    } light;

    // planet-specific properties
    struct Planet {
        bool useDayTexture = false;
        bool useNightTexture = false;
        bool useGlossTexture = false;
        bool useCloudsTexture = false;
        std::shared_ptr<QOpenGLTexture> dayTexture;
        std::shared_ptr<QOpenGLTexture> nightTexture;
        std::shared_ptr<QOpenGLTexture> glossTexture;
        std::shared_ptr<QOpenGLTexture> cloudsTexture;
        float night_scale = 1.0;
        float night_blend_exp = 3.0;
        bool debug = false;
        bool debugWaterLand = false;
        bool animateClouds = false;
    } planet;

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
    void apply() override;

};


#endif // MATERIAL_H
