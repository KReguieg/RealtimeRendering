#pragma once

#include <QWidget>

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QTimer>
#include <QOpenGLFramebufferObject>

#include "mesh/mesh.h"
#include "camera.h"
#include "node.h"
#include "nodenavigator.h"

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

    QMatrix4x4& worldTransform() { return nodes_["World"]->transformation; }

signals:

    void displayBufferContents(unsigned int id, QString label, const QImage& img);

public slots:

    // change background color
    void setBackgroundColor(QVector3D rgb);

    // methods to change common material parameters
    void toggleAnimation(bool flag);
    void setLightIntensity(size_t i, float v);
    void setAmbientScale(float v);
    void setDiffuseScale(float v);
    void setSpecularScale(float v);
    void setShininess(float v);

    // methods affecting post processing
    void setPostFilterKernelSize(int n);
    void useDepthOfField();
    void useTwoPassGauss();
    void toggleJittering(bool value);
    void toggleSplitDisplay(bool value);
    void toggleFBODisplay(bool value);

    // change the node to be rendered in the scene
    void setSceneNode(QString node);

    // pass key/mouse events to navigators
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    // perform OpenGL rendering of the entire scene. Don't call this yourself.
    void draw();

    // trigger a redraw of the widget through this method
    void update();

    // adjust camera / viewport / ... if drawing surface changes
    void updateViewport(size_t width, size_t height);

protected:

    // draw the actual scene for one pass of a multi-pass algorithm
    void draw_scene_();

    // draw from FBO for post processing, use full viewport
    void post_draw_full_(QOpenGLFramebufferObject& fbo, Node& node);
    // draw from FBO, render left half of node1 + right half of node2
    void post_draw_split_(QOpenGLFramebufferObject &fbo1, Node &node1, QOpenGLFramebufferObject &fbo2, Node &node2);

    // multi-pass rendering
    std::shared_ptr<QOpenGLFramebufferObject> fbo1_, fbo2_;
    std::map<QString, std::shared_ptr<PostMaterial>> post_materials_;
    bool split_display_ = true;
    bool show_FBOs_ = false;

    // parent widget
    QWidget* parent_;

    // periodically update the scene for animations
    QTimer timer_;

    // track time for animations
    std::chrono::high_resolution_clock clock_;
    std::chrono::time_point<std::chrono::high_resolution_clock> firstDrawTime_;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastDrawTime_;

    // rotation
    double angle = 0.0;
    bool rotationOn = true;

    // bg color
    QVector3D bgcolor_ = QVector3D(0.4f,0.4f,0.4f);

    // different materials to be demonstrated
    std::map<QString, std::shared_ptr<TexturedPhongMaterial>> materials_;

    // mesh(es) to be used / shared
    std::map<QString, std::shared_ptr<Mesh>> meshes_;

    // nodes to be used
    std::map<QString, std::shared_ptr<Node>> nodes_;

    // light nodes for any number of lights
    std::vector<std::shared_ptr<Node>> lightNodes_;

    // navigation
    std::unique_ptr<ModelTrackball> navigator_;
    std::unique_ptr<PositionNavigator> lightNavigator_;
    std::unique_ptr<RotateCameraY> cameraNavigator_;

    // helper for creating programs from shader files
    std::shared_ptr<QOpenGLShaderProgram> createProgram(const std::string& vertex,
                                                        const std::string& fragment,
                                                        const std::string& geom = "");

    // helper for creating a node scaled to size 1
    std::shared_ptr<Node> createNode(std::shared_ptr<Mesh> mesh, bool scale_to_1 = true);

    // helpers to construct the objects and to build the hierarchical scene
    void makeNodes();
    void makeScene();

};

