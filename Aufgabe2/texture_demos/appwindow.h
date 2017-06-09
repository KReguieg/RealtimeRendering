#pragma once

#include <QWidget>
#include <QSettings>

#include "scene.h"

#include <QAudioInput>

namespace Ui {
class AppWindow;
}

/*
 *   RTR demo App, AppWindow
 *   Author: Hartmut Schirmacher
 *
 *   The AppWindow holds the GUI of the application that has been created
 *   and can be edited in Qt UI designer.
 *
 *   It consists of an OpenGL Widget for rendering the OpenGL Scene,
 *   plus additional UI elements like buttons.
 *
 *   It also acts as controller that connects app-wide keyboard events
 *   with respective actions in the scene.
 *
 */

class AudioInfo : public QIODevice
{
    Q_OBJECT

public:
    AudioInfo(const QAudioFormat &format, QObject *parent);
    ~AudioInfo();

    void start();
    void stop();

    qreal level() const { return m_level; }

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

private:
    const QAudioFormat m_format;
    quint32 m_maxAmplitude;
    qreal m_level; // 0.0 <= m_level <= 1.0

signals:
    void update();
};

class RenderArea : public QWidget
{
    Q_OBJECT

public:
    RenderArea(QWidget *parent = 0);

    void setLevel(qreal value);

protected:
    void paintEvent(QPaintEvent *event);

private:
    qreal m_level;
    QPixmap m_pixmap;
};


class AppWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AppWindow(QWidget *parent = 0);
    ~AppWindow();

    // convenience shortcut to the OpenGL scene
    Scene &scene();

public slots:

    /* show buttons etc, and a border around the OpenGL widget */
    void showUI();
    /* hide all buttons etc, and remove border around the OpenGL widget */
    void hideUI();

    /* process app-wide key events and trigger respective actions in UI or in the scene */
    void keyPressEvent(QKeyEvent *event) override;

    /* set default values for the UI widgets. must only be called AFTER constructor */
    void setDefaultUIValues();

protected:

    /* when closing, the window stores its geometry in the settings */
    void closeEvent(QCloseEvent *event) override;

    /* when showing, initialize values of UI widgets */
    void showEvent(QShowEvent *event) override;

private slots:
    void deviceChanged(int number);
    void refreshDisplay();
    void readMore();
private:
    void initializeAudio();
    void createAudioInput();
    // this is the connection to the class that will come out of the UI designer
    Ui::AppWindow *ui;

    // remember if the window has been shown / initialized already
    bool wasInitialized = false;

    // this is where the app remembers its settings, i.e. window size
    QSettings settings_;

    RenderArea *m_canvas;
    QAudioDeviceInfo m_device;
    AudioInfo *m_audioInfo;
    QAudioFormat m_format;
    QAudioInput *m_audioInput;
    QIODevice *m_input;
    QByteArray m_buffer;
};

