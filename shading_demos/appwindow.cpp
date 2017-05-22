/*
 *   Implementation of class AppWindow for the RTR demo App
 *   Author: Hartmut Schirmacher
 *
 */

#include <assert.h>

#include <QApplication>
#include <QKeyEvent>

#include "appwindow.h"
#include "ui_appwindow.h"
#include "scene.h"

AppWindow::AppWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppWindow)
{
    // connects what comes out of the Qt designer to this class
    ui->setupUi(this);

    // quit button -> quit app
    connect(ui->quitButton, &QPushButton::clicked,
            []{ qApp->quit(); });

    // combo box to change the model
    connect(ui->modelComboBox, &QComboBox::currentTextChanged,
            [this](const QString& txt) { scene().changeModel(txt); } );

    // combo box to change the shader
    connect(ui->shaderComboBox, &QComboBox::currentTextChanged, [this](const QString& txt) {

        // change shader
        scene().changeShader(txt);

        // random color button only makes sense for a specific material
        ui->randomColorButton->setEnabled(txt == "user-defined color");

    } );
    ui->randomColorButton->setEnabled(false); // initial state

    // checkbox to start/stop animation
    connect(ui->animationCheckbox, &QCheckBox::stateChanged,
            [this](bool onOrOff) { scene().toggleAnimation(onOrOff); } );

    // trigger generation of new random color
    connect(ui->randomColorButton, &QCheckBox::pressed,
            [this]() { scene().setNewRandomColor(); } );


}

AppWindow::~AppWindow()
{
    delete ui;
}

Scene &AppWindow::scene()
{
    return ui->openGLWidget->scene();
}

void AppWindow::showUI()
{
    ui->ui_container->show();
    // default pixel margins (on Mac)
    ui->appWindowLayout->setContentsMargins(12,12,12,12);

}

void AppWindow::hideUI()
{
    ui->ui_container->hide();
    // zero pixel margins, for borderless OpenGL window
    ui->appWindowLayout->setContentsMargins(0,0,0,0);
}

void AppWindow::keyPressEvent(QKeyEvent *event)
{
    assert(event);

    // specific key events the app reacts to
    switch(event->key()) {

    // key 'h': show/hide UI
    case Qt::Key_H:
        ui->ui_container->isHidden()? showUI() : hideUI();
        break;

    // key 'q': quit app
    case Qt::Key_Q:
        qApp->quit();
        break;

    // rotate world
    case Qt::Key_Left:
        scene().worldTransform().rotate(-5, QVector3D(0,1,0));
        ui->openGLWidget->update();
        break;
    case Qt::Key_Right:
        scene().worldTransform().rotate(5, QVector3D(0,1,0));
        ui->openGLWidget->update();
        break;

    // translate camera
    case Qt::Key_Up:
        scene().camera().translateViewPoint(QVector3D(0,0,-0.1f));
        ui->openGLWidget->update();
        break;
    case Qt::Key_Down:
        scene().camera().translateViewPoint(QVector3D(0,0,+0.1f));
        ui->openGLWidget->update();
        break;

    } // switch
}
