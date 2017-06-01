
#include "rtrglwidget.h"
#include "scene.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>

#include <iostream>

using namespace std;

rtrGLWidget::rtrGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
}

rtrGLWidget::~rtrGLWidget()
{
}

QSize rtrGLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize rtrGLWidget::sizeHint() const
{
    return QSize(200, 200);
}

void rtrGLWidget::initializeGL()
{

    // this has to be done before any other GL call is issued
    initializeOpenGLFunctions();

    // debut output: supported GLSL version
    const unsigned char* glsl_ver = glGetString(GL_SHADING_LANGUAGE_VERSION);
    cout << "GLSL version: " << glsl_ver << endl;

    // create and initialize logger
    logger = new QOpenGLDebugLogger(this);
    logger->initialize();

    // extension required to do logging :-(
    if(!context()->hasExtension(QByteArrayLiteral("GL_KHR_debug"))) {
        // cout << "OpenGL debug extension not available" << endl;
    } else {
        cout << "OpenGL debug extension available" << endl;
        // start logging
        // connect(logger, &QOpenGLDebugLogger::messageLogged, [](const QOpenGLDebugMessage &msg) {
        //     qDebug() << msg;
        // });
        // logger->startLogging();
    }

    // scene will call OpenGL functions, so it cannot be
    // instantiated before we have initialized OpenGL
    scene_ = make_shared<Scene>(this, context());

}

void rtrGLWidget::paintGL()
{
    scene_->draw();
}

void rtrGLWidget::resizeGL(int width, int height)
{
    scene_->updateViewport(width,height);
}

void rtrGLWidget::mousePressEvent(QMouseEvent*)
{
    cout << "Mouse Press" << endl;
}

void rtrGLWidget::mouseMoveEvent(QMouseEvent*)
{
    // int dx = event->x() - lastMousePos_.x();
    // if (event->buttons() & Qt::LeftButton)
}

void rtrGLWidget::mouseReleaseEvent(QMouseEvent* )
{
    emit clicked();
}
