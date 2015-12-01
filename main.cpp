#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "keyboard.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    Keyboard keyboard;
    //Keyboard::setReciever(&engine);
    engine.rootContext()->setContextProperty("keyboard", &keyboard);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    //Keyboard::setReciever(engine.rootObjects().first());
    Keyboard::setReciever(&engine);

    return app.exec();
}

