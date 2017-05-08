#pragma once

#include <QWidget>

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

#include "mesh/mesh.h"
#include "camera.h"
#include "node.h"


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

    Camera& camera() { return *camera_; }
    QMatrix4x4& worldTransform() { return worldTransform_; }

signals:

public slots:

    // change model according to combo box in UI
    void changeModel(const QString& txt);

    // change the shading level accroding to sliderBox in UI
    void changeShadingLevel(const  float& shadingLevels);

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
    std::map<QString, std::shared_ptr<Material>> materials_;

    // mesh(es) to be used / shared
    std::map<QString, std::shared_ptr<Mesh>> meshes_;

    // nodes to be used
    std::map<QString, std::shared_ptr<Node>> nodes_;

    // current node to be rendered
    std::shared_ptr<Node> currentNode_;

    // current camera
    std::shared_ptr<Camera> camera_;

    // world transformation
    QMatrix4x4 worldTransform_;

    // helper for creating programs from shader files
    std::shared_ptr<QOpenGLShaderProgram> createProgram(const std::string& vertex,
                                                        const std::string& fragment);

    // helper for creating a node scaled to size 1
    std::shared_ptr<Node> createNode(std::shared_ptr<Mesh> mesh, bool scale_to_1 = true);

    //
    std::shared_ptr<PhongMaterial> material_;
};

