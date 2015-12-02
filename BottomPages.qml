import QtQuick 2.5
import "qrc:/Controls"

Item {
    id: bottomPages
    property int pageNum: 0

    property alias pageWidth: referencePage.width
    property int pageHeight: referencePage.height

    property real inactiveOpacity: 0.2
    property bool hideInactives: false

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
                x: 100
                y: 100
                width: 300
                height: 40
            }

            Button {
                id: but
                width: 300
                height: 70
                text: "een"
                anchors.centerIn: parent
                opacity: (hideInactives && !isPressed) ? inactiveOpacity : 1

                Binding {
                    target: bottomPages
                    property: "hideInactives"
                    value: but.isPressed
                }
            }

            /*Button {
                id: but2
                anchors.left: but.left
                anchors.right: but.right
                anchors.top: but.bottom
                anchors.topMargin: 5
                width: but.width
                height: but.height
                text: "een2"
                opacity: (hideInactives && !isPressed) ? inactiveOpacity : 1

                Binding {
                    target: bottomPages
                    property: "hideInactives"
                    value: but2.isPressed
                }
            }*/

            TextBox {
                x: 100
                y: 500
                width: 300
                height: 40
            }

            TextBox {
                x: 100
                y: 450
                width: 300
                height: 40
            }
        }

        Item {
            id: slicePage
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: modelPage.right
            width: pageWidth

            Button {
                width: 300
                height: 70
                text: "twee"
                anchors.centerIn: parent
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

