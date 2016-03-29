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
            font.pixelSize: 35
        }

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: title.bottom
            anchors.topMargin: 15
            anchors.bottom: btnCancel.top
            anchors.bottomMargin: 10
            color: "transparent"
            border.color: Style.accentColor
            border.width: 2

            ListModel {
                id: filesModel
                ListElement { displayName: "Zipped up"; glyph: "Archive.png" }
                ListElement { displayName: "Random folder"; glyph: "Folder.png" }
                ListElement { displayName: "Stupid folder"; glyph: "Folder.png" }
                ListElement { displayName: "MyModel"; glyph: "STL.png" }
                ListElement { displayName: "USB Drive"; glyph: "USB.png" }
                ListElement { displayName: "SD Card"; glyph: "SD.png" }
            }

            Component {
                id: fileDelegate

                Rectangle {
                    id: fileRect
                    color: (_mouseArea.pressed) ? Style.pressedRed : Style.bgRed
                    width: parent.width
                    height: 60

                    Text {
                        anchors.centerIn: parent
                        text: displayName
                        font.family: 'Nevis'
                        font.pixelSize: 20
                        color: Style.textColor
                    }

                    Image {
                        anchors.left: parent.left
                        anchors.leftMargin: 5
                        anchors.verticalCenter: parent.verticalCenter
                        width: 50
                        height: 50
                        source: glyph
                    }

                    MouseArea {
                        id: _mouseArea
                        anchors.fill: parent

                        onClicked: {
                            // TODO: alert C++
                        }
                    }
                }
            }

            ListView {
                id: fileListView
                anchors.fill: parent
                anchors.margins: 7
                clip: true
                model: filesModel
                delegate: fileDelegate
                spacing: 5
            }
        }

        Button {
            id: btnCancel
            text: "Cancel"
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            onClicked: {
                // TODO: alert C++
                fbRect.close()
            }
        }
    }
}
