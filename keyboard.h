#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QObject>

class Keyboard : public QObject
{
    Q_OBJECT

private:
    static bool m_open;
    static int openRequests;
    static QObject *m_receiver;
    static void emitKey(int key, QString keyText = NULL);

public:
    explicit Keyboard(QObject *parent = 0);
    static void setReciever(QObject *receiver);

    Q_INVOKABLE static void pressKey(QString c);
    Q_INVOKABLE static void pressRight();
    Q_INVOKABLE static void pressLeft();
    Q_INVOKABLE static void pressEnter();
    Q_INVOKABLE static void pressSpace();

    Q_INVOKABLE static void requestOpen();
    Q_INVOKABLE static void requestClose();
    // Force close

    Q_PROPERTY(bool open READ open WRITE setOpen NOTIFY openChanged)
    void setOpen(bool a);
    bool open();

signals:
   void openChanged();

public slots:
};

#endif // KEYBOARD_H
