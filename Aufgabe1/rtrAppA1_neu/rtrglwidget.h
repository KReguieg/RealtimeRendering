/*
 *  OpenGL Widget: handles GL init, paint, and resize events.
 *
 *  Initialized and holds the scene.
 *  Should be used to control mouse events.
 *
 *  see also: http://doc.qt.io/qt-5/qopenglwidget.html
 */

#pragma once

#include <memory> // std::unique_ptr, std::shared_ptr

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLDebugLogger>

// forward declaration
class Scene;
class QOpenGLShaderProgram;
class QOpenGLTexture;

// QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
// QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class rtrGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit rtrGLWidget(QWidget *parent = 0);
    ~rtrGLWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    // reference to the actual scene that is drawn
    Scene& scene() { return *scene_; }

signals:
    void clicked();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:

    // scene to be drawn and/or manipulated
    std::shared_ptr<Scene> scene_;

    QOpenGLDebugLogger *logger;
};

