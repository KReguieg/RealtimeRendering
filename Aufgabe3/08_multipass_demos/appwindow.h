#pragma once

#include <QWidget>
#include <QSettings>
#include <QVBoxLayout>

#include "scene.h"
#include "imagedisplaybutton.h"


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

    /*
     *   Adds or updates a button with a thumbnail image (i.e. for some FBO content)
     *   When clicking the button, a new window is opened that allows to view
     *   the full-resolution image.
     *
     *   The ids must be continuous integers starting at 0. You must add images
     *   starting with id 0, then id 1, ...
     *
     *   If you do not follow the above rule, ordering of widgets within the UI
     *   may be garbeled, or worse things may happen.
     *
     *   Once you have added images in the right order, you may update them
     *   in arbitrary order.
     *
     */
    void displayBufferContents(unsigned int id, QString label, const QImage& img);
    void hideBufferContents();

protected:

    /* when closing, the window stores its geometry in the settings */
    void closeEvent(QCloseEvent *event) override;

    /* when showing, initialize values of UI widgets */
    void showEvent(QShowEvent *event) override;

private slots:

private:
    // this is the connection to the class that will come out of the UI designer
    Ui::AppWindow *ui;

    // remember if the window has been shown / initialized already
    bool wasInitialized = false;

    // thumbnails that act as buttons to view the FBO contents
    std::map<int,ImageDisplayButton*> buttons_;

    // widget in which we add all the FBO buttons
    QVBoxLayout* fbo_widget_layout_;

    // this is where the app remembers its settings, i.e. window size
    QSettings settings_;
};

