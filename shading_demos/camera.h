#pragma once

#include <QMatrix4x4>
#include <QOpenGLShaderProgram>

#include "material.h"

#include <string> // std::string

/*
 *  Simple perspective camera with methods that support a "look at" model.
 *  to manipulate the camera based on a certain interaction model.
 *
 *  Use setMatrices() for setting uniform values in the shader
 *  to the matrices that represent the camera.
 *
 *  Under which uniform names will the matrices be available in the
 *  shader? See default values in method setMatrixUniformNames().
 *
 */

class Camera
{
public:

    /*
     *  Constructor setting all values.
     *  If you do not specify any arguments, a default camera
     *  is constructed that looks from (0,0,2) to (0,0,0)
     *  with a vertical field of view of 45°.
     *
     */
    explicit Camera(QVector3D from = QVector3D(0,0,2),
                    QVector3D to = QVector3D(0,0,0),
                    QVector3D up = QVector3D(0,1,0),
                    float fovy = 45.0,
                    float aspect_ratio = 1.0,
                    float near = 0.1,
                    float far_plane = 10.0);

    // query projection parameters
    float fieldOfView() const { return fovy_; }
    float aspectRatio() const { return aspect_ratio_; }

    // manipulate projection parameters
    void setAspectRatio(float aspect);
    void setFieldOfView(float degrees);

    // change view point, keep look at point
    void translateViewPoint(QVector3D delta);

    /*
     *  Set OpenGL's model, view, modelview, normal, projection and
     *  model-view-projection matrices.
     *
     *  Always apply before rendering the actual objects.
     *
     */
    void setMatrices(QOpenGLShaderProgram &prog,
                     QMatrix4x4 modelMatrix = QMatrix4x4()) const;
    void setMatrices(Material& material,
                     QMatrix4x4 modelMatrix = QMatrix4x4()) const;

    /*
     *  Set the uniform names to which the camera matrices will be bound
     *  in future calls of setMatrices() for this camera object.
     */
    void setMatrixUniformNames(const std::string m    = "modelMatrix",
                               const std::string v    = "viewMatrix",
                               const std::string p    = "projectionMatrix",
                               const std::string mv   = "modelViewMatrix",
                               const std::string n    = "normalMatrix",
                               const std::string mvp  = "modelViewProjectionMatrix");


private:

    // view + projection matrices
    QMatrix4x4 viewMatrix_;
    QMatrix4x4 projectionMatrix_;

    // parameters for re-calculating the view using a look-at model
    QVector3D from_, to_, up_;

    // parameters for re-calculating the projection
    float fovy_, aspect_ratio_, near_, far_;

    // re-calculate internal matrices
    void calculateMatrices_();

    // uniform names for the affected matrices
    std::string name_m_, name_v_, name_p_, name_mv_, name_n_, name_mvp_;

};

