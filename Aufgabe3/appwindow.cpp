/*
 *   Implementation of class AppWindow for the RTR demo App
 *   Author: Hartmut Schirmacher
 *
 */

#include <assert.h>

#include <QApplication>
#include <QKeyEvent>
#include <QDesktopWidget>
#include <QDialog>

#include "appwindow.h"
#include "ui_appwindow.h"
#include "scene.h"

#include "imagedisplaydialog.h"

AppWindow::AppWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppWindow)
{
    // connects what comes out of the Qt designer to this class
    ui->setupUi(this);

    // set up scroll area

    auto fbo_widget = new QWidget();
    ui->fbo_area->setWidget(fbo_widget);
    ui->fbo_area->setWidgetResizable(true);
    fbo_widget->setLayout(fbo_widget_layout_ = new QVBoxLayout());

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

    // pass 1
    connect(ui->blackBgRadioButton, &QRadioButton::clicked,
            [this](bool) { scene().setBackgroundColor(QVector3D(0,0,0)); } );
    connect(ui->greyBgRadioButton, &QRadioButton::clicked,
            [this](bool) { scene().setBackgroundColor(QVector3D(0.4f,0.4f,0.4f)); } );
    connect(ui->whiteBgRadioButton, &QRadioButton::clicked,
            [this](bool) { scene().setBackgroundColor(QVector3D(1,1,1)); } );
    connect(ui->light0Slider, &QSlider::valueChanged,
            [this](int value) { scene().setLightIntensity(0, float(value)/100.0); } );
    connect(ui->modelComboBox, &QComboBox::currentTextChanged,
            [this](QString value) { scene().setSceneNode(value); } );

    connect(ui->ambientSlider, &QSlider::valueChanged,
            [this](int value) { scene().setAmbientScale(float(value)/20.0); } );
    connect(ui->diffuseSlider, &QSlider::valueChanged,
            [this](int value) { scene().setDiffuseScale(float(value)/20.0); } );
    connect(ui->specularSlider, &QSlider::valueChanged,
            [this](int value) { scene().setSpecularScale(float(value)/20.0); } );
    connect(ui->shininessSlider, &QSlider::valueChanged,
            [this](int value) { scene().setShininess(float(value)); } );

    // post processing parameters -------------------------------
    connect(ui->postFilterComboBox, &QComboBox::currentTextChanged,
            [this](QString value) {
        if(value == "Blur") {
            hideBufferContents();
            ui->post_kernel_size->setEnabled(true);
            scene().useDepthOfField();
        } else if(value == "2-Pass 9x9 Gauss") {
            scene().useTwoPassGauss();
            hideBufferContents();
            ui->post_kernel_size->setDisabled(true);
        }
    } );
    connect(ui->splitScreenCheckbox, &QCheckBox::toggled,
            [this](bool value) { scene().toggleSplitDisplay(value); } );
    connect(ui->showFBOtoggle, &QCheckBox::toggled,
            [this](bool value) {
        scene().toggleFBODisplay(value);
        if(!value)
            hideBufferContents();
    } );
    connect(ui->jitterCheckbox, &QCheckBox::toggled,[this](bool value){ scene().toggleJittering(value); });

    // strange cast here: see https://stackoverflow.com/questions/16794695/connecting-overloaded-signals-and-slots-in-qt-5
    connect(ui->post_kernel_size, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [this](int value) { scene().setPostFilterKernelSize(value); } );

}

void AppWindow::displayBufferContents(unsigned int id, QString label, const QImage &img)
{

    qDebug() << "showing FBO, id = " << id;
    auto pixmap = QPixmap::fromImage(img);

    // if button for this ID already exists, update it; else create new
    if(buttons_.find(id) == buttons_.end())
        buttons_[id] = new ImageDisplayButton(this,pixmap,label);
    else
        buttons_[id]->update(pixmap, label);

    // if empty layout, add spacer to push things upwards
    if(fbo_widget_layout_->count()==0)
        fbo_widget_layout_->addSpacerItem(new QSpacerItem(1,100,QSizePolicy::Minimum, QSizePolicy::Expanding));

    // do we still need to add the button to the UI / layout
    assert(int(id) <= fbo_widget_layout_->count()-1);
    if(fbo_widget_layout_->count()-1==int(id))
        fbo_widget_layout_->insertWidget(id,buttons_[id]);

}

void AppWindow::hideBufferContents()
{
    for(auto w : buttons_)
        delete w.second;
    buttons_.clear();
    update();
}

// called when initially shown
void AppWindow::setDefaultUIValues() {

    // connection from scene to here has to be done once the
    //   scene atually exists
    bool b = connect(&(this->scene()), &Scene::displayBufferContents,
                     this, &AppWindow::displayBufferContents);
    assert(b);

    // scene rendering parameters
    ui->light0Slider->setValue(85.0);
    ui->ambientSlider->setValue(20);
    ui->diffuseSlider->setValue(20);
    ui->specularSlider->setValue(0);
    ui->shininessSlider->setValue(20);
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

    switch(event->key()) {

    // key 'h': show/hide UI
    case Qt::Key_H:
        ui->ui_container->isHidden()? showUI() : hideUI();
        return;

    // key 'q': quit app
    case Qt::Key_Q:
        close();
        return;

    } // switch

    // pass events on to scene
    scene().keyPressEvent(event);
}
