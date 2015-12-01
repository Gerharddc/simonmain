#include "keyboard.h"

#include <QKeyEvent>
#include <QCoreApplication>
#include <QQuickItem>
#include <QGuiApplication>
#include <QKeySequence>

#ifdef QT_DEBUG
#include <QDebug>
#endif

QObject *Keyboard::m_receiver = NULL;
bool Keyboard::m_open  = false;
int Keyboard::openRequests = 0;
Keyboard *Keyboard::singleton = NULL;

Keyboard::Keyboard(QObject *parent) : QObject(parent)
{
    singleton = this;
}

void Keyboard::setOpen(bool a)
{
    if (a != m_open) {
        m_open = a;

        if (singleton != NULL)
            emit singleton->openChanged();
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
    #ifdef QT_DEBUG
    qDebug() << "Pressing key: " << c[0];
    #endif

    emitKey(QKeySequence(c)[0], c);
}

void Keyboard::pressRight()
{
    #ifdef QT_DEBUG
    qDebug() << "Pressing right";
    #endif

    emitKey(Qt::Key_Right);
}

void Keyboard::pressLeft()
{
    #ifdef QT_DEBUG
    qDebug() << "Pressing left";
    #endif

    emitKey(Qt::Key_Left);
}

void Keyboard::pressEnter()
{
    #ifdef QT_DEBUG
    qDebug() << "Pressing enter";
    #endif

    emitKey(Qt::Key_Enter);
}

void Keyboard::pressSpace()
{
    #ifdef QT_DEBUG
    qDebug() << "Pressing space";
    #endif

    emitKey(Qt::Key_Space, " "); //With " " the word Space is entered
}

void Keyboard::pressBackspace()
{
    #ifdef QT_DEBUG
    qDebug() << "Pressing backspace";
    #endif

    emitKey(Qt::Key_Backspace, " "); //With " " the word Space is entered
}

void Keyboard::requestOpen()
{
    #ifdef QT_DEBUG
    qDebug() << "Requested open";
    #endif

    openRequests++;

    setOpen(true);
}

void Keyboard::requestClose()
{
    #ifdef QT_DEBUG
    qDebug() << "Requested close";
    #endif

    openRequests--;

    setOpen(openRequests > 0);
}

void Keyboard::forceClose()
{
    #ifdef QT_DEBUG
    qDebug() << "Requested force close";
    #endif

    // TODO: maybe there is a safer way
    openRequests = 1;
    qobject_cast<QQuickItem*>(QGuiApplication::focusObject())->setFocus(false);
}



