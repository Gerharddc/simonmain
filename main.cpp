#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QFontDatabase>
#include <QQuickItem>

#include <QDebug>

#include "keyboard.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QFontDatabase database;
    qDebug() << "Re: " << database.addApplicationFont(":/nevis.ttf");
    qDebug() << "Re: " << database.addApplicationFont(":/Roboto-Regular.ttf");

    QQuickView view;

    Keyboard keyboard;
    view.rootContext()->setContextProperty("keyboard", &keyboard);

    view.setSource(QUrl(QStringLiteral("qrc:/main.qml")));

    QObject *root = view.rootObject();//engine.rootObjects()[0];
    QObject *qmlKeyboard = root->findChild<QObject*>("keyboard");
    Keyboard::setQmlKeyboard(qobject_cast<QQuickItem*>(qmlKeyboard));

    view.show();

    return app.exec();
}

