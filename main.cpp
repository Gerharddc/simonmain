#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QFontDatabase>
#include <QQuickItem>

#include "Keyboard/keyboard.h"
#include "Rendering/glhelper.h"
//#include "Rendering/loadedgl.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

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
    Keyboard::setQmlKeyboard(qobject_cast<QQuickItem*>(qmlKeyboard));

    view.show();

    return app.exec();
}

