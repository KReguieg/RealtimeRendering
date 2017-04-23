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

#include <iostream>

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
        scene().camera().translateViewPoint(QVector3D(0,0,-0.1));
        ui->openGLWidget->update();
        break;
    case Qt::Key_Down:
        scene().camera().translateViewPoint(QVector3D(0,0,+0.1));
        ui->openGLWidget->update();
        break;

    } // switch
}

void AppWindow::on_horizontalSlider_Y_sliderMoved(int position)
{
    scene().worldTransform().rotate(position, QVector3D(0,1,0));
    ui->openGLWidget->update();
}

void AppWindow::on_horizontalSlider_X_sliderMoved(int position)
{
    scene().worldTransform().rotate(position, QVector3D(1,0,0));
    ui->openGLWidget->update();
}

void AppWindow::on_horizontalSlider_Z_sliderMoved(int position)
{
    scene().worldTransform().rotate(position, QVector3D(0,0,1));
    ui->openGLWidget->update();
}

void AppWindow::on_spinBox_shadingLevel_valueChanged(int shadingLevel)
{
    // Get the value from the ui and pass it to the scene
    // Where Uniforms can be set.
    std::cout << "Value: " << float(shadingLevel) << endl;
    //scene().program_->setUniformValue("shadingLevles", float(shadingLevel));
    //scene().changeShadingLevel((float)shadingLevel);

    ui->openGLWidget->update();
}
