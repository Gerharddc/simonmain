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

    Keyboard {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
    }

    Item {
        visible: true

        TextBox {
            x: 121
            y: 195
            width: 200
        }

        DropTextBox {
            id: textBox1
            x: 121
            y: 269
            width: 212
            options: ListModel {
                ListElement { option: "Jan" }
                ListElement { option: "Koos" }
                ListElement { option: "Piet" }
            }
        }

        ProgressBar {
            id: progressBar1
            x: 100
            y: 498
            width: 293
            height: 50
        }

        Button {
            id: button1
            x: 46
            y: 604
            width: 147
            height: 59
            text: "Klein"

            onClicked: {
                progressBar1.value -= 10
            }
        }

        Button {
            id: button2
            x: 246
            y: 604
            width: 147
            height: 59
            text: "Groot"

            onClicked: progressBar1.value += 10
        }

        Slider {
            id: slider1
            x: 37
            y: 70
            width: 274
            height: 63
        }
    }
}

