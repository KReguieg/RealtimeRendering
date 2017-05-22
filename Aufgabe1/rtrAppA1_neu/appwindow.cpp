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
#include <QColorDialog>
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
        scene().camera().translateViewPoint(QVector3D(0,0,-0.1f));
        ui->openGLWidget->update();
        break;
    case Qt::Key_Down:
        scene().camera().translateViewPoint(QVector3D(0,0,+0.1f));
        ui->openGLWidget->update();
        break;

    } // switch
}

void AppWindow::on_spinBox_cel_level_valueChanged(int shadingLevel)
{
    scene().changeShadingLevel((float)shadingLevel);
    ui->openGLWidget->update();
}

void AppWindow::on_horizontalSlider_x_rotation_valueChanged(int value)
{
    scene().worldTransform().rotate(value*36, QVector3D(0,1,0));
    ui->openGLWidget->update();
}

void AppWindow::on_spinBox_radius_valueChanged(int radius)
{
    scene().changeDotRadius(float(radius));
    ui->openGLWidget->update();
}


void AppWindow::on_spinBox_density_valueChanged(int density)
{
    scene().changeDotDensity(float(density));
    ui->openGLWidget->update();
}

void AppWindow::on_pushButton_colorPicker_clicked()
{
    std::cout << "Clicked on the nice color picker button!" << std::endl;
    QColor tempCol = QColorDialog::getColor(QColor::fromRgba(circle_color_), this, "Pick a color");
    // std::cout << "Selected the nice color " + tempCol.name().toStdString() << std::endl;
    scene().changeDotColor(QVector4D(tempCol.red()/255.0, tempCol.green()/255.0, tempCol.blue()/255.0, 1));
    ui->pushButton_colorPicker->setStyleSheet("background-color: " + tempCol.name());
    ui->openGLWidget->update();
}

void AppWindow::on_horizontalSlider_y_rotation_valueChanged(int value)
{
    scene().worldTransform().rotate(value*36, QVector3D(1,0,0));
    ui->openGLWidget->update();
}
