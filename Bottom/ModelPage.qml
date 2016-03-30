import QtQuick 2.3
import "qrc:/Controls"
import "qrc:/StyleSheet.js" as Style

BottomPage {
    Item {
        anchors.fill: parent
        anchors.margins: 20        

        Button {
            id: btnAdd
            text: "Add model"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 5
            isDimmable: true

            onClicked: {
                rootRect.addStl()
            }
        }

        Text {
            id: _text
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: btnAdd.bottom
            anchors.topMargin: 15
            font.family: 'Nevis'
            color: Style.textColor
            text: 'Select some models to see more options'
            font.pixelSize: 20
            visible: renderer.meshesSelected === 0
        }

        Button {
            id: btnRemove
            text: "Remove selected models"
            anchors.top: btnAdd.bottom
            anchors.topMargin: 15
            anchors.left: parent.left
            anchors.right: parent.right
            isDimmable: true
            visible: renderer.meshesSelected > 0

            onClicked: {
                renderer.removeSelectedMeshes()
            }
        }

        /*DropTextBox {
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
        }*/
    }
}

