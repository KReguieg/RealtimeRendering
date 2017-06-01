/*
 *   Implementation of class AppWindow for the RTR demo App
 *   Author: Hartmut Schirmacher
 *
 */

#include <assert.h>

#include <QApplication>
#include <QKeyEvent>
#include <QDesktopWidget>

#include "appwindow.h"
#include "ui_appwindow.h"
#include "scene.h"

AppWindow::AppWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppWindow)
{
    // connects what comes out of the Qt designer to this class
    ui->setupUi(this);

    // window title (app name is set in main.cpp)
    this->setWindowTitle(QCoreApplication::applicationName());

    // have we saved the size of the app window, then restore that size
    bool restored = restoreGeometry(settings_.value("window_geometry").toByteArray());

    // else set sensible initial size
    if(!restored) {

        // available part of the desktop (minus dock/taskbar etc.)
        QDesktopWidget desktop;
        QRect avail = desktop.availableGeometry(desktop.primaryScreen());

        // default size = 70% of available desktop
        setGeometry(50,50,avail.width()*0.7, avail.height()*0.7);
    }

    // quit button -> close app window
    connect(ui->quitButton, &QPushButton::clicked, [this]{ close(); });

    // when last window closes, quit
    connect(qApp, &QGuiApplication::lastWindowClosed, []{ qApp->quit(); });

    // combo box to change the model
    connect(ui->modelComboBox, &QComboBox::currentTextChanged,
            [this](const QString& txt) { scene().changeModel(txt);
    } );

    // combo box to change the shader
    connect(ui->shaderComboBox, &QComboBox::currentTextChanged, [this](const QString& txt) {
        scene().changeShader(txt);
        if(txt == "+Clouds")
            ui->animationCheckbox->setVisible(true);
        else
            ui->animationCheckbox->setVisible(false);

    } );

    // main shader parameters
    connect(ui->blackBgRadioButton, &QRadioButton::clicked,
            [this](bool) { scene().setBackgroundColor(QVector3D(0,0,0)); } );
    connect(ui->greyBgRadioButton, &QRadioButton::clicked,
            [this](bool) { scene().setBackgroundColor(QVector3D(0.4f,0.4f,0.4f)); } );
    connect(ui->whiteBgRadioButton, &QRadioButton::clicked,
            [this](bool) { scene().setBackgroundColor(QVector3D(1,1,1)); } );
    connect(ui->sunlightSlider, &QSlider::valueChanged,
            [this](int value) { scene().setLightIntensity(float(value)/100.0); } );
    connect(ui->nightLightSlider, &QSlider::valueChanged,
            [this](int value) { scene().setNightScale(float(value)/100.0); } );
    connect(ui->blendExpSlider, &QSlider::valueChanged,
            [this](int value) { scene().setBlendExponent(float(value)/100.0); } );
    connect(ui->animationCheckbox, &QCheckBox::stateChanged,
            [this](bool onOrOff) { scene().toggleAnimation(onOrOff); } );
    connect(ui->bumpMapCheckbox, &QCheckBox::stateChanged,
            [this](bool onOrOff) { scene().toggleBumpMapping(onOrOff); } );
    connect(ui->dispMapCheckBox, &QCheckBox::stateChanged,
            [this](bool onOrOff) { scene().toggleDisplacementMapping(onOrOff); } );
    connect(ui->bumpMapSlider, &QSlider::valueChanged,
            [this](int value) { scene().setBumpMapScale(float(value)/100.0); } );
    connect(ui->dispMapSlider, &QSlider::valueChanged,
            [this](int value) { scene().setDisplacementMapScale(float(value)/100.0); } );

    // wireframe / vector shaders
    connect(ui->wireframeCheckBox, &QCheckBox::stateChanged,
            [this](bool onOrOff) { scene().toggleWireframe(onOrOff); } );
    connect(ui->vectorScaleSlider, &QSlider::valueChanged,
            [this](int value) { scene().setVectorScale(float(value)/100.0); } );

    // from Stack overflow, syntax is ... great ...
    connect(ui->vectorsGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            [this](int which) {
        scene().visualizeVectors(which);
    });

}

// called when initially shown
void AppWindow::setDefaultUIValues() {

    ui->sunlightSlider->setValue(85.0);
    ui->nightLightSlider->setValue(20.0);
    ui->blendExpSlider->setValue(30.0);
    ui->bumpMapSlider->setValue(10.0);
    ui->dispMapSlider->setValue(10.0);
    ui->vectorScaleSlider->setValue(10.0);
    ui->greyBgRadioButton->setChecked(true);

}

void AppWindow::closeEvent(QCloseEvent *event)
{
    settings_.setValue("window_geometry", saveGeometry());
    QWidget::closeEvent(event); // let parent class do its job
}

void AppWindow::showEvent(QShowEvent *event)
{
    if(!wasInitialized) {
        setDefaultUIValues();
        wasInitialized = true;
    }
    QWidget::showEvent(event); // hand to parent
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
    ui->mainLayout->setContentsMargins(12,12,12,12);

}

void AppWindow::hideUI()
{
    ui->ui_container->hide();
    // zero pixel margins, for borderless OpenGL window
    ui->mainLayout->setContentsMargins(0,0,0,0);
}

void AppWindow::keyPressEvent(QKeyEvent *event)
{
    assert(event);

    // qDebug() << "modifier: " << event->modifiers() << " key code: " << event->key();

    // keys with Shift pressed
    if(event->modifiers() & Qt::ShiftModifier) {

        switch(event->key()) {

        // move light
        case Qt::Key_Left:
            scene().moveLight(-0.1f,0);
            break;
        case Qt::Key_Right:
            scene().moveLight(0.1f,0);
            break;
        case Qt::Key_Up:
            scene().moveLight(0,0.1f);
            break;
        case Qt::Key_Down:
            scene().moveLight(0,-0.1f);
            break;
        }
        return;

    }

    // keys without Shift pressed
    switch(event->key()) {

    // key 'h': show/hide UI
    case Qt::Key_H:
        ui->ui_container->isHidden()? showUI() : hideUI();
        break;

    // key 'q': quit app
    case Qt::Key_Q:
        close();
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
