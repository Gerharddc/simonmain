import QtQuick 2.3
import "StyleSheet.js" as Style
import "qrc:/Controls"

Rectangle {
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
    }

    TextBox {
        x: 121
        y: 195
        width: 200
        //height: 50
    }

    DropTextBox {
        id: textBox1
        x: 121
        y: 269
        width: 212
        //height: 59
        options: ListModel {
            ListElement { option: "Jan" }
            ListElement { option: "Koos" }
            ListElement { option: "Piet" }
        }
    }
}

