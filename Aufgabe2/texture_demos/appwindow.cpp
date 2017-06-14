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

#include <stdlib.h>
#include <math.h>
#include <qendian.h>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QPainter>
#include <QDateTime>
#include <QDebug>

const int BufferSize = 4096;


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

    m_canvas = new RenderArea(this);
    ui->verticalLayout_5->addWidget(m_canvas);

    const QAudioDeviceInfo &defaultDeviceInfo = QAudioDeviceInfo::defaultInputDevice();
    ui->comboBox->addItem(defaultDeviceInfo.deviceName(), qVariantFromValue(defaultDeviceInfo));
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioInput)) {
        if (deviceInfo != defaultDeviceInfo)
            ui->comboBox->addItem(deviceInfo.deviceName(), qVariantFromValue(deviceInfo));
    }
    connect(ui->comboBox, SIGNAL(activated(int)), SLOT(deviceChanged(int)));

    initializeAudio();
}

RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    m_level = 0;
    setMinimumHeight(30);
    setMinimumWidth(200);
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    painter.setPen(Qt::black);
    painter.drawRect(QRect(painter.viewport().left()+10,
                           painter.viewport().top()+10,
                           painter.viewport().right()-20,
                           painter.viewport().bottom()-20));
    if (m_level == 0.0)
        return;

    int pos = ((painter.viewport().right()-20)-(painter.viewport().left()+11))*m_level;
    painter.fillRect(painter.viewport().left()+11,
                     painter.viewport().top()+10,
                     pos,
                     painter.viewport().height()-21,
                     Qt::red);
}

void RenderArea::setLevel(qreal value)
{
    m_level = value;
    update();
}



AudioInfo::AudioInfo(const QAudioFormat &format, QObject *parent)
    :   QIODevice(parent)
    ,   m_format(format)
    ,   m_maxAmplitude(0)
    ,   m_level(0.0)

{
    switch (m_format.sampleSize()) {
    case 8:
        switch (m_format.sampleType()) {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 255;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 127;
            break;
        default:
            break;
        }
        break;
    case 16:
        switch (m_format.sampleType()) {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 65535;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 32767;
            break;
        default:
            break;
        }
        break;

    case 32:
        switch (m_format.sampleType()) {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 0xffffffff;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 0x7fffffff;
            break;
        case QAudioFormat::Float:
            m_maxAmplitude = 0x7fffffff; // Kind of
        default:
            break;
        }
        break;

    default:
        break;
    }
}

AudioInfo::~AudioInfo()
{
}

void AudioInfo::start()
{
    open(QIODevice::WriteOnly);
}

void AudioInfo::stop()
{
    close();
}

qint64 AudioInfo::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)

    return 0;
}

qint64 AudioInfo::writeData(const char *data, qint64 len)
{
    if (m_maxAmplitude) {
        Q_ASSERT(m_format.sampleSize() % 8 == 0);
        const int channelBytes = m_format.sampleSize() / 8;
        const int sampleBytes = m_format.channelCount() * channelBytes;
        Q_ASSERT(len % sampleBytes == 0);
        const int numSamples = len / sampleBytes;

        quint32 maxValue = 0;
        const unsigned char *ptr = reinterpret_cast<const unsigned char *>(data);

        for (int i = 0; i < numSamples; ++i) {
            for (int j = 0; j < m_format.channelCount(); ++j) {
                quint32 value = 0;

                if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
                    value = *reinterpret_cast<const quint8*>(ptr);
                } else if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::SignedInt) {
                    value = qAbs(*reinterpret_cast<const qint8*>(ptr));
                } else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qFromLittleEndian<quint16>(ptr);
                    else
                        value = qFromBigEndian<quint16>(ptr);
                } else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::SignedInt) {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qAbs(qFromLittleEndian<qint16>(ptr));
                    else
                        value = qAbs(qFromBigEndian<qint16>(ptr));
                } else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qFromLittleEndian<quint32>(ptr);
                    else
                        value = qFromBigEndian<quint32>(ptr);
                } else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::SignedInt) {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qAbs(qFromLittleEndian<qint32>(ptr));
                    else
                        value = qAbs(qFromBigEndian<qint32>(ptr));
                } else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::Float) {
                    value = qAbs(*reinterpret_cast<const float*>(ptr) * 0x7fffffff); // assumes 0-1.0
                }

                maxValue = qMax(value, maxValue);
                ptr += channelBytes;
            }
        }

        maxValue = qMin(maxValue, m_maxAmplitude);
        m_level = qreal(maxValue) / m_maxAmplitude;
    }

    emit update();
    return len;
}


void AppWindow::initializeAudio()
{
    m_format.setSampleRate(44100);
    m_format.setChannelCount(1);
    m_format.setSampleSize(32);
    m_format.setSampleType(QAudioFormat::SignedInt);
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setCodec("audio/pcm");

    m_device = QAudioDeviceInfo::defaultInputDevice();

    QAudioDeviceInfo info(m_device);
    if (!info.isFormatSupported(m_format)) {
        qWarning() << "Default format not supported - trying to use nearest";
        m_format = info.nearestFormat(m_format);
    }

    //if (m_audioInfo)
    //    delete m_audioInfo;

    m_audioInfo  = new AudioInfo(m_format, this);
    connect(m_audioInfo, SIGNAL(update()), SLOT(refreshDisplay()));
    qWarning() << "rying to use nearest";
    createAudioInput();
}

void AppWindow::createAudioInput()
{
    m_audioInput = new QAudioInput(m_device, m_format, this);
    //qreal initialVolume = QAudio::convertVolume(m_audioInput->volume(),
    //                                            QAudio::LinearVolumeScale,
    //                                            QAudio::LogarithmicVolumeScale);
    //m_volumeSlider->setValue(qRound(initialVolume * 100));
    m_audioInfo->start();
    m_audioInput->start(m_audioInfo);
}

void AppWindow::readMore()
{
    if (!m_audioInput){
        qDebug() << "w";
        return;
    }
    qint64 len = m_audioInput->bytesReady();
    if (len > BufferSize)
        len = BufferSize;
    qint64 l = m_input->read(m_buffer.data(), len);
    if (l > 0)
        m_audioInfo->write(m_buffer.constData(), l);
}
void AppWindow::refreshDisplay()
{
    m_canvas->setLevel(m_audioInfo->level());
}
void AppWindow::deviceChanged(int index)
{
    m_audioInfo->stop();
    m_audioInput->stop();
    m_audioInput->disconnect(this);
    delete m_audioInput;

    m_device = ui->comboBox->itemData(index).value<QAudioDeviceInfo>();
    initializeAudio();
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
void AppWindow::keyReleaseEvent(QKeyEvent *event)
{
    assert(event);

    scene().SetInput(QVector2D(0, 0));
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

    case Qt::Key_W:
        scene().SetInput(QVector2D(0,1));
        break;
    case Qt::Key_S:
        scene().SetInput(QVector2D(0,-1));
        break;
    case Qt::Key_A:
        scene().SetInput(QVector2D(1,0));
        break;
    case Qt::Key_D:
        scene().SetInput(QVector2D(-1,0));
        break;
    } // switch
}
