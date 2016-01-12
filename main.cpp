#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QFontDatabase>
#include <QQuickItem>

#include "Keyboard/keyboard.h"
#include "Rendering/fborenderer.h"

#include "Rendering/structures.h"
#include "Rendering/stlimporting.h"

#include <QFile>
#include <QString>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

#ifdef ANDROID
#define FILECOUNT 3
    QString* names = new QString[FILECOUNT];
    names[0] = "bin.stl";
    names[1] = "cube.vsh";
    names[2] = "cube.fsh";
    for (uint8_t i = 0; i < FILECOUNT; i++)
    {
        QFile dfile("assets:/" + names[i]);
        if (dfile.exists())
        {
            QString name = "./" + names[i];
            dfile.copy(name);
            QFile::setPermissions(name, QFile::ReadOwner);
        }
    }
    delete[] names;
#endif

    //FBORenderer::SetSTLMesh(STLImporting::ImportSTL("block.ast"));
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

