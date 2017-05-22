#ifndef MATERIAL_H
#define MATERIAL_H

#include <QOpenGLShaderProgram>

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

/*
 * Phong illumination model with a single point light and
 * a light in camera coordinates (headlight)
 *
 */
class PhongMaterial : public Material {
public:

    // constructor requires existing shader program
    PhongMaterial(std::shared_ptr<QOpenGLShaderProgram> prog) : Material(prog) {}

    // actual properties of the object's materia
    QVector3D k_ambient  = QVector3D(0.50f,0.10f,0.10f); // red-ish
    QVector3D k_diffuse  = QVector3D(0.50f,0.10f,0.10f); // red-ish
    QVector3D k_specular = QVector3D(0.80f,0.80f,0.80f); // white-ish
    float     shininess  = 80; // middle-ish

    // light position: up right from the camera, in eye coordinates
    QVector4D lightPos_EC    = QVector4D(2,2,0,1);
    QVector3D lightIntensity = QVector3D(1,1,1);

    // ambient light
    QVector3D ambientLightIntensity = QVector3D(0.3f,0.3f,0.3f);

    // bind underlying shader program and set required uniforms
    void apply() override;
};

// dummy material: no parameters to be set.
class DummyMaterial : public Material {
public:

    DummyMaterial(std::shared_ptr<QOpenGLShaderProgram> prog) : Material(prog) {}
    void apply() override;

};

// uniform material: can choose constant color.
class UniformMaterial : public Material {
public:

    UniformMaterial(std::shared_ptr<QOpenGLShaderProgram> prog) : Material(prog) {}
    void apply() override;

    QVector3D myUniformColor  = QVector3D(0,1,0);

};

// time material: time parameter for animations.
class TimeMaterial : public Material {
public:

    TimeMaterial(std::shared_ptr<QOpenGLShaderProgram> prog) : Material(prog) {}
    void apply() override;

    float time = 0.0;
};




#endif // MATERIAL_H
