import QtQuick 2.3
import "qrc:/Controls"

BottomPage {
    Item {
        anchors.fill: parent
        anchors.margins: 20

        TextBox {
            id: tb1
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            isDimmable: true
        }

        Button {
            id: btn1
            text: "Add model"
            anchors.top: tb1.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 15
            isDimmable: true

            onClicked: {
                rootRect.addStl()
            }
        }

        DropTextBox {
            anchors.top: btn1.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 15
            isDimmable: true
            options: ListModel {
                            ListElement { option: "Een" }
                            ListElement { option: "Twee" }
                            ListElement { option: "Drie" }
                            ListElement { option: "Vier" }
                            ListElement { option: "Vyf" }
                        }
        }
    }
}

