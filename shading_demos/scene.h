#pragma once

#include <QWidget>

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QTimer>

#include "mesh/mesh.h"
#include "camera.h"
#include "node.h"


#include <memory> // std::unique_ptr
#include <map>    // std::map
#include <chrono> // clock, time calculations

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

    // change shader program
    void changeShader(const QString& txt);

    // set random new color for UniformMaterial
    void setNewRandomColor();

    // toggle timer / animation
    void toggleAnimation(bool flag);

    // perform OpenGL rendering of the entire scene. Don't call this yourself.
    void draw();

    // trigger a redraw of the widget through this method
    void update() { parent_->update(); }

    // adjust camera / viewport / ... if drawing surface changes
    void updateViewport(size_t width, size_t height);

protected:

    // parent widget
    QWidget* parent_;

    // periodically update the scene for animations
    QTimer timer_;

    // track time for animations
    std::chrono::high_resolution_clock clock_;
    std::chrono::time_point<std::chrono::high_resolution_clock> firstDrawTime_;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastDrawTime_;

    // different materials to be demonstrated
    std::shared_ptr<PhongMaterial>   phongMaterial_;
    std::shared_ptr<UniformMaterial> uniformMaterial_;
    std::shared_ptr<TimeMaterial>    animationMaterial_;
    std::shared_ptr<DummyMaterial>   minimalMaterial_, normalsMaterial_,
                                     deform1Material_, deform2Material_,
                                     texcoordMaterial_;

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
};

