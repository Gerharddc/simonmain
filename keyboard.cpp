#include "keyboard.h"

#include <QDebug>
#include <QKeyEvent>
#include <QCoreApplication>
#include <QQuickItem>
#include <QGuiApplication>
#include <QKeySequence>

QObject *Keyboard::m_receiver = NULL;
bool Keyboard::m_open  = false;
int Keyboard::openRequests = 0;

Keyboard::Keyboard(QObject *parent) : QObject(parent)
{

}

void Keyboard::setOpen(bool a)
{
    if (a != m_open) {
        m_open = a;
        emit openChanged();
    }
}

bool Keyboard::open()
{
    return m_open;
}

void Keyboard::setReciever(QObject *receiver)
{
    m_receiver = receiver;
}

void Keyboard::emitKey(int key, QString keyText)
{
    QQuickItem* receiver = qobject_cast<QQuickItem*>(QGuiApplication::focusObject());

    if(!receiver) {
        return;
    }

    if (keyText == NULL) {
        keyText = QKeySequence(key).toString();
    }

    QKeyEvent* pressEvent = new QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyText);
    QKeyEvent* releaseEvent = new QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier);
    QCoreApplication::sendEvent(receiver, pressEvent);
    QCoreApplication::sendEvent(receiver, releaseEvent);
}

void Keyboard::pressKey(QString c)
{
    qDebug() << "Pressing key: " << c[0];

    emitKey(QKeySequence(c)[0], c);
}

void Keyboard::pressRight()
{
    qDebug() << "Pressing right";

    emitKey(Qt::Key_Right);
}

void Keyboard::pressLeft()
{
    qDebug() << "Pressing left";

    emitKey(Qt::Key_Left);
}

void Keyboard::pressEnter()
{
    qDebug() << "Pressing enter";

    emitKey(Qt::Key_Enter);
}

void Keyboard::pressSpace()
{
    qDebug() << "Pressing space";

    emitKey(Qt::Key_Space);
}

void Keyboard::requestOpen()
{
    qDebug() << "Requested open";
}

void Keyboard::requestClose()
{
    qDebug() << "Requested close";
}



