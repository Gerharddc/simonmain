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

CONFIG(SIMON):DEFINES += ROTATE_SCREEN
CONFIG(SIMON):DEFINES += GLES
DEFINES += QT_APPLICATION

DISTFILES +=
