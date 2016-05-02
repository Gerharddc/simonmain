import QtQuick 2.3
import "qrc:/Controls"

BottomPage {
    id: printPage

    Item {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 20
        anchors.rightMargin: 20

        Button {
            id: btnPrint
            text: printer.printing ? "Stop printing" : "Print model"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 5
            isDimmable: true

            onClicked: {
                if (printer.printing)
                    printer.stopPrint()
                else
                    renderer.printToolpath()
            }
        }

        Button {
            id: btnHAll
            text: "Home all"
            anchors.top: btnPrint.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 15
            isDimmable: true
            enabled: !printer.printing

            onClicked: {
                printer.homeAll()
            }
        }

        Row {
            id: rowHome
            spacing: 10
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: btnHAll.bottom
            anchors.topMargin: 15

            Button {
                width: (parent.width - 20) / 3
                text: "Home X"
                isDimmable: true
                enabled: !printer.printing

                onClicked: {
                    printer.homeX()
                }
            }

            Button {
                width: (parent.width - 20) / 3
                text: "Home Y"
                isDimmable: true
                enabled: !printer.printing

                onClicked: {
                    printer.homeY()
                }
            }

            Button {
                width: (parent.width - 20) / 3
                text: "Home Z"
                isDimmable: true
                enabled: !printer.printing

                onClicked: {
                    printer.homeZ()
                }
            }
        }

        Row {
            id: rowMove
            spacing: 10
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: rowHome.bottom
            anchors.topMargin: 25

            Item {
                width: (parent.width - 20) / 3
                height: childrenRect.height

                Label {
                    id: txtX
                    text: 'X Dist:'
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    isDimmable: true
                }

                TextBox {
                    id: tboxX
                    anchors.top: txtX.bottom
                    anchors.topMargin: 5
                    anchors.left: parent.left
                    anchors.right: parent.right
                    isDimmable: true
                    text: "0"
                }
            }

            Item {
                width: (parent.width - 20) / 3
                height: childrenRect.height

                Label {
                    id: txtY
                    text: 'Y Dist:'
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    isDimmable: true
                }

                TextBox {
                    id: tboxY
                    anchors.top: txtY.bottom
                    anchors.topMargin: 5
                    anchors.left: parent.left
                    anchors.right: parent.right
                    isDimmable: true
                    text: "0"
                }
            }

            Item {
                width: (parent.width - 20) / 3
                height: childrenRect.height

                Label {
                    id: txtZ
                    text: 'Z Dist:'
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    isDimmable: true
                }

                TextBox {
                    id: tboxZ
                    anchors.top: txtZ.bottom
                    anchors.topMargin: 5
                    anchors.left: parent.left
                    anchors.right: parent.right
                    isDimmable: true
                    text: "0"
                }
            }
        }

        Button {
            id: btnMove
            text: "Move all"
            anchors.top: rowMove.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 10
            isDimmable: true
            enabled: !printer.printing

            onClicked: {
                printer.move(tboxX.text, tboxY.text, tboxZ.text)
            }
        }

        Label {
            anchors.top: btnMove.bottom
            anchors.topMargin: 15
            isDimmable: true
            text: "Target temparture:"
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
