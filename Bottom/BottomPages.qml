import QtQuick 2.3
import "qrc:/Controls"
import "qrc:/DimmLogic.js" as DimmLogic

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

            Flickable {
                anchors.fill: parent
                contentWidth: width
                //contentHeight: height + 100
                contentHeight: childrenRect.height
                clip: true

                TextBox {
                    x: 100
                    y: 100
                    width: 200
                    isDimmable: true
                }

                Button {
                    width: 200
                    text: "een"
                    x: 100
                    y: 200
                    isDimmable: true
                }

                DropTextBox {
                    x: 100
                    y: 300
                    width: 250
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
                isDimmable: true
            }

            Button {
                width: 300
                text: "twee"
                anchors.centerIn: parent
                isDimmable: true
            }

            ProgressBar {
                x: 100
                y: 450
                width: 300
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
                text: "drie"
                anchors.centerIn: parent
            }
        }
    }
}

