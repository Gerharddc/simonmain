TEMPLATE = app

QT += qml quick
CONFIG += c++11

SOURCES += main.cpp \
    Keyboard/keyboard.cpp \
    Rendering/loadedgl.cpp \
    Rendering/fborenderer.cpp \
    Rendering/stlrenderer.cpp \
    Rendering/stlimporting.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    Keyboard/keyboard.h \
    Rendering/glhelper.h \
    Rendering/loadedgl.h \
    Rendering/fborenderer.h \
    Rendering/stlrenderer.h \
    Rendering/mathhelper.h \
    Rendering/structures.h \
    Misc/strings.h \
    Rendering/stlimporting.h

INCLUDEPATH += $$PWD/glm

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

CONFIG(SIMON):DEFINES += ROTATE_SCREEN
CONFIG(SIMON):DEFINES += GLES
DEFINES += QT_APPLICATION

# - setup the correct location to install to and load from
android {
    # android platform
    # From: http://community.kde.org/Necessitas/Assets
    SAMPLES_INSTALL_PATH = /assets
    DEFINES += GLES
    DEFINES += ANDROID
} else {
    # other platforms
    SAMPLES_INSTALL_PATH = $$OUT_PWD
}

SAMPLE_FILES = \
    bin.stl \
    GL/cube.vsh \
    GL/cube.fsh

# - setup the 'make install' step
samples.path = $$SAMPLES_INSTALL_PATH
samples.files += $$SAMPLE_FILES
samples.depends += FORCE

INSTALLS += samples
