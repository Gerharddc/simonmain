import QtQuick 2.3
import "qrc:/Controls"

BottomPage {
    id: slicePage

    Item {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 20
        anchors.rightMargin: 20

        Button {
            id: btnSlice
            text: renderer.slicerRunning ? "Stop slicing" : "Start slicing"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 5
            isDimmable: true

            onClicked: {
                renderer.sliceMeshes()
            }
        }

        Label {
            id: txtSave
            text: 'Satus: ' + renderer.slicerStatus
            anchors.top: btnSlice.bottom
            anchors.topMargin: 15
            anchors.left: parent.left
            anchors.right: parent.right
            isDimmable: true
        }

        ListModel {
            id: settingsModel
            ListElement { title: "Bed Width"; setting: "bedWidth"; }
            ListElement { title: "Bed Length"; setting: "bedLength"; }
            ListElement { title: "Bed Height"; setting: "bedHeight"; }
            ListElement { title: "Infill Speed"; setting: "infillSpeed"; }
        }

        Component {
            id: settingsDelegate

            Item {
                width: parent.width
                height: childrenRect.height

                Label {
                    id: setLabel
                    text: title + ":"
                    width: parent.width
                }

                TextBox {
                    id: setTBox
                    anchors.top: setLabel.bottom
                    anchors.topMargin: 5
                    width: parent.width

                    Binding on text {
                        when: !setTBox.isActive
                        value: settings[setting]
                    }

                    Binding {
                        target: settings
                        property: setting
                        value: setTBox.text
                    }
                }
            }
        }

        Column {
            id: settingsColumn
            spacing: 10
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: txtSave.bottom
            anchors.topMargin: 15

            Repeater {
                model: settingsModel
                delegate: settingsDelegate
            }
        }
    }

    /*Slider {
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
    }*/
}
