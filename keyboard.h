#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QObject>
#include <QQuickItem>

class Keyboard : public QObject
{
    Q_OBJECT

private:
    static bool m_open;
    static void emitKey(int key, QString keyText = NULL);
    static Keyboard *singleton;
    static int m_uiOffset;
    static QQuickItem *qmlKeyboard;
    const static int m_keyboardHeight = 350;
    static QQuickItem *focusedItem;
    static bool m_shifted;

public:
    explicit Keyboard(QObject *parent = 0);

    Q_INVOKABLE static void pressKey(QString c);
    Q_INVOKABLE static void pressRight();
    Q_INVOKABLE static void pressLeft();
    Q_INVOKABLE static void pressEnter();
    Q_INVOKABLE static void pressSpace();
    Q_INVOKABLE static void pressBackspace();
    Q_INVOKABLE static void pressShift();

    Q_INVOKABLE static void requestOpen(QQuickItem *item);
    Q_INVOKABLE static void requestClose(QQuickItem *item);
    Q_INVOKABLE static void forceClose();

    static void setQmlKeyboard(QQuickItem *item);

    Q_PROPERTY(bool open READ open WRITE setOpen NOTIFY openChanged)
    static void setOpen(bool a);
    static bool open();

    Q_PROPERTY(int keyboardHeight READ keyboardHeight NOTIFY keyboardHeightChanged)
    static int keyboardHeight() { return m_keyboardHeight; }

    Q_PROPERTY(int uiOffset READ uiOffset NOTIFY uiOffsetChanged)
    static int uiOffset() { return m_uiOffset; }

    Q_PROPERTY(bool shifted READ shifted WRITE setShifted NOTIFY shiftedChanged)
    static bool shifted() { return m_shifted; }
    static void setShifted(bool a);

signals:
   void openChanged();
   void keyboardHeightChanged();
   void uiOffsetChanged();
   void shiftedChanged();

public slots:
};

#endif // KEYBOARD_H
