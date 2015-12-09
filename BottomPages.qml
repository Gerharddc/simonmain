import QtQuick 2.3
import "qrc:/Controls"
import "DimmLogic.js" as DimmLogic

Item {
    id: bottomPages
    property int pageNum: 0

    property alias pageWidth: referencePage.width
    property int pageHeight: referencePage.height

    Item {
        id: referencePage
        anchors.fill: parent
    }

    Item {
        id: pageContainer

        width: 3 * pageWidth
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.leftMargin: -(pageNum * pageWidth)

        Behavior on anchors.leftMargin {
            PropertyAnimation {}
        }

        Item {
            id: modelPage
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            width: pageWidth

            TextBox {
                id: text1
                x: 100
                y: 100
                width: 300
                height: 40
                isDimmable: true
            }

            Button {
                id: but
                width: 300
                height: 70
                text: "een"
                anchors.centerIn: parent
                isDimmable: true
            }

            DropTextBox {
                id: text2
                x: 100
                y: 500
                width: 300
                height: 40
                isDimmable: true
                options: ListModel {
                                ListElement { option: "Jan" }
                                ListElement { option: "Koos" }
                                ListElement { option: "Piet" }
                            }
            }

            TextBox {
                id: text3
                x: 100
                y: 450
                width: 300
                height: 40
                isDimmable: true
            }
        }

        Item {
            id: slicePage
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: modelPage.right
            width: pageWidth

            Slider {
                x: 100
                y: 100
                width: 300
                height: 40
                isDimmable: true
            }

            Button {
                width: 300
                height: 70
                text: "twee"
                anchors.centerIn: parent
                isDimmable: true
            }

            ProgressBar {
                x: 100
                y: 450
                width: 300
                height: 40
                isDimmable: true
            }
        }

        Item {
            id: printPage
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: slicePage.right
            width: pageWidth

            Button {
                width: 300
                height: 70
                text: "drie"
                anchors.centerIn: parent
            }
        }
    }
}

