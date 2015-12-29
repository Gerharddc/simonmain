#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QFontDatabase>
#include <QQuickItem>

#include "Keyboard/keyboard.h"
#include "Rendering/fborenderer.h"

#include "Rendering/structures.h"
#include "Rendering/stlimporting.h"

int Hisher(int hmmm)
{
    return hmmm;
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    FBORenderer::SetSTLMesh(STLImporting::ImportSTL("bin.stl"));

    qmlRegisterType<FBORenderer>("FBORenderer", 1, 0, "Renderer");

    QFontDatabase database;
    database.addApplicationFont(":/nevis.ttf");

    QQuickView view;

    Keyboard keyboard;
    view.rootContext()->setContextProperty("keyboard", &keyboard);

#ifdef ROTATE_SCREEN
#define ROOTQML "qrc:/Rotated.qml"
#else
#define ROOTQML "qrc:/MainScreen.qml"
#endif

    view.setSource(QUrl(QStringLiteral(ROOTQML)));

    QObject *root = view.rootObject();
    QObject *qmlKeyboard = root->findChild<QObject*>("keyboard");
    keyboard.setQmlKeyboard(qobject_cast<QQuickItem*>(qmlKeyboard));

    view.show();

    return app.exec();
}

