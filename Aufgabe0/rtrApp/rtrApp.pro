# PROJECT FILE FOR RTRAPP

# We want the most current C++ standard.
# i.e. for std::make_unique
CONFIG += c++14

# HEADERS TO BE PROCESSED BY QT
HEADERS      += \
                appwindow.h \
    rtrglwidget.h \
    objloader.h \
    mesh.h \
    scene.h \
    camera.h \
    indexbuffer.h \
    vertexbuffer.h \
    bbox.h \
    material.h

# C++ SOURCE FILES TO BE COMPILED AND LINKED TOGETHER
SOURCES      += \
                main.cpp \
                appwindow.cpp \
    rtrglwidget.cpp \
    objloader.cpp \
    mesh.cpp \
    scene.cpp \
    camera.cpp \
    indexbuffer.cpp \
    bbox.cpp \
    material.cpp

# QT MODULES TO BE USED
QT           += gui widgets

# RESOURCE FILES TO BE PROCESSED BY QT
RESOURCES    += \
    assets.qrc

# UI DEFINITION FILES TO BE PROCESSED BY QT (FROM DESIGNER APP)
FORMS        += \
                appwindow.ui

DISTFILES += \
    assets/textures/RTR-ist-super-4-3.png \
    assets/shaders/myphong.frag \
    assets/shaders/myphong.vert \
    assets/shaders/minimal.frag \
    assets/shaders/minimal.vert

