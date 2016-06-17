TEMPLATE = app

QT += qml quick serialport
CONFIG += c++11

SOURCES += main.cpp \
    Keyboard/keyboard.cpp \
    Rendering/loadedgl.cpp \
    Rendering/fborenderer.cpp \
    Rendering/stlimporting.cpp \
    Rendering/gcodeimporting.cpp \
    Rendering/glhelper.cpp \
    Rendering/structures.cpp \
    Rendering/comborendering.cpp \
    Misc/globalsettings.cpp \
    Misc/filebrowser.cpp \
    Rendering/stlrendering.cpp \
    Rendering/toolpathrendering.cpp \
    Rendering/gridrendering.cpp \
    Rendering/stlexporting.cpp \
    Misc/qtsettings.cpp \
    Printer/printer.cpp \
    ChopperEngine/clipper.cpp \
    ChopperEngine/chopperengine.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

HEADERS += \
    Keyboard/keyboard.h \
    Rendering/glhelper.h \
    Rendering/loadedgl.h \
    Rendering/fborenderer.h \
    Rendering/mathhelper.h \
    Rendering/structures.h \
    Misc/strings.h \
    Rendering/stlimporting.h \
    Rendering/gridgeneration.h \
    Rendering/gcodeimporting.h \
    Rendering/comborendering.h \
    Misc/globalsettings.h \
    Misc/filebrowser.h \
    Rendering/stlrendering.h \
    Rendering/toolpathrendering.h \
    Rendering/gridrendering.h \
    Rendering/stlexporting.h \
    Misc/delegate.h \
    Misc/qtsettings.h \
    Printer/printer.h \
    ChopperEngine/clipper.hpp \
    ChopperEngine/chopperengine.h

INCLUDEPATH += $$PWD/glm

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

CONFIG(SIMON):DEFINES += ROTATE_SCREEN
CONFIG(SIMON):DEFINES += GLES
DEFINES += QT_APPLICATION

SAMPLE_FILES = \
    GL/mesh.vsh \
    GL/minimal.fsh \
    GL/line.vsh \
    GL/filament.vsh \
    GL/filament.fsh \

# - setup the correct location to install to and load from
android {
    # android platform
    # From: http://community.kde.org/Necessitas/Assets
    SAMPLES_INSTALL_PATH = /assets
    DEFINES += GLES
    DEFINES += ANDROID
} else {
    SIMON {
        target.path = $$OUT_PWD
        INSTALLS += target
    }
    SAMPLES_INSTALL_PATH = $$OUT_PWD
}

# - setup the 'make install' step
samples.path = $$SAMPLES_INSTALL_PATH
samples.files += $$SAMPLE_FILES

INSTALLS += samples
