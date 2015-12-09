import QtQuick 2.3
import "StyleSheet.js" as Style
import "qrc:/Controls"

Rectangle {
    id: rootRect
    visible: true
    width: 480
    height: 800
    color: Style.bgMain

    TopDrawer {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        z: 100;
    }

    BottomDrawer {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        anchors.bottomMargin: keyboard.uiOffset

        Behavior on anchors.bottomMargin {
            PropertyAnimation { }
        }
    }

    Keyboard {
        objectName: "keyboard"
        anchors.left: parent.left
        anchors.bottom: parent.bottom
    }
}

