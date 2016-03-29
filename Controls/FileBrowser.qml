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

            Component {
                id: fileDelegate

                Rectangle {
                    id: fileRect
                    color: (_mouseArea.pressed) ? Style.pressedRed : Style.bgRed
                    width: parent.width
                    height: 60

                    Text {
                        anchors.centerIn: parent
                        text: model.modelData.displayName
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
                        source: model.modelData.glyph
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
                model: fileBrowser.fileModel
                delegate: fileDelegate
                spacing: 5

                Component.onCompleted: {
                    fileBrowser.getRootDirectory()
                }

                onModelChanged: {
                    console.log("Changed")
                    console.log(model)

                    for (var i = 0; i < model.length; i++)
                        for (var prop in model[i])
                            console.log("Object item:", prop, "=", model[i][prop])
                }
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
