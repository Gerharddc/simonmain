#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QQuickItem>

#include "keyboard.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    Keyboard keyboard;
    engine.rootContext()->setContextProperty("keyboard", &keyboard);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    QObject *root = engine.rootObjects()[0];
    QObject *qmlKeyboard = root->findChild<QObject*>("keyboard");
    Keyboard::setQmlKeyboard(qobject_cast<QQuickItem*>(qmlKeyboard));

    return app.exec();
}

