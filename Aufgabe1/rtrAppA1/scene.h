#pragma once

#include <QWidget>

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

#include "mesh.h"
#include "camera.h"
#include "material.h"

#include <memory> // std::unique_ptr
#include <map>    // std::map

/*
 * OpenGL-based scene. Required objects are created in the constructor,
 * and the scene is rendered using render().
 *
 * Also, the scene derives from QObject so it can use Qt's
 * signal and slot mechanism to connect to the app's UI.
 *
 * Do not call render() directly, use update() instead.
 *
 */

class Scene : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit Scene(QWidget* parent, QOpenGLContext *context);
    ~Scene();

    Camera& camera() { return *camera_; }
    QMatrix4x4& worldTransform() { return worldTransform_; }

signals:

public slots:

    // change model according to combo box in UI
    void changeModel(const QString& txt);

    // change the shading level accroding to sliderBox in UI
    // void changeShadingLevel(const  GLfloat& shadingLevels);

    // perform OpenGL rendering of the entire scene. Don't call this yourself.
    void draw();

    // trigger a redraw of the widget through this method
    void update() { parent_->update(); }

    // adjust camera / viewport / ... if drawing surface changes
    void updateViewport(size_t width, size_t height);

protected:

    // parent widget
    QWidget* parent_;

    // program(s) to be used
    QOpenGLShaderProgram* program_;

    // mesh(es) to be used / shared
    std::map<QString, std::shared_ptr<Mesh>> meshes_;
    std::shared_ptr<Mesh> currentMesh_;

    // camera(s) to be used / shared
    std::shared_ptr<Camera> camera_;

    // world transformation
    QMatrix4x4 worldTransform_;

    // material
    std::shared_ptr<PhongMaterial> material_;


};

