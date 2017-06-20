// Simply demo app for Real Time Rendering
// Author: Hartmut Schirmacher

#include <QApplication>
#include <QSurfaceFormat>

#include "appwindow.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // these values will determine under which key settings are stored
    QCoreApplication::setOrganizationName("Beuth Hochschule");
    QCoreApplication::setOrganizationDomain("beuth-hochschule.de");
    QCoreApplication::setApplicationName("RTR multipass demos ***");

#if 1
    // format to require OpenGL 3.2 and the right kinds of buffers
    QSurfaceFormat format;
    format.setMajorVersion(3);
    format.setMinorVersion(2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setOption(QSurfaceFormat::DebugContext); // enable OpenGL debugging!!!

    // format.setDepthBufferSize(24);
    // make this the default format for all OpenGL widgets to be created
    QSurfaceFormat::setDefaultFormat(format);
#endif

    AppWindow window;
    window.show();

    return app.exec();
}
