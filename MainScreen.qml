import QtQuick 2.3
import "StyleSheet.js" as Style

Rectangle {
    visible: true
    width: 480
    height: 800
    color: "black"

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
    }
}

