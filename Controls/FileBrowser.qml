import QtQuick 2.3
import "qrc:/StyleSheet.js" as Style

Rectangle {
    id: fbRect
    color: Style.bgMain
    visible: (opacity == 0) ? false : true
    opacity: 0

    function close() {
        fbRect.opacity = 0
    }

    function open() {
        fbRect.opacity = 1
    }

    Behavior on opacity {
        PropertyAnimation {}
    }

    Item {
        anchors.fill: parent
        anchors.margins: 20

        Text {
            id: title
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            font.family: 'Nevis'
            color: Style.accentColor
            text: 'Choose a file to load'
            font.pixelSize: 30
        }

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: title.bottom
            anchors.topMargin: 10
            anchors.bottom: btnCancel.top
            anchors.bottomMargin: 10
            color: "transparent"
            border.color: Style.accentColor
            border.width: 2
        }

        Button {
            id: btnCancel
            text: "Cancel"
            width: 200
            anchors.left: parent.left
            anchors.bottom: parent.bottom

            onClicked: {
                // TODO: alert C++
                fbRect.close()
            }
        }

        Button {
            id: btnOk
            text: "Ok"
            width: 200
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            onClicked: {
                // TODO: alert c++
                fbRect.close()
            }
        }
    }
}
