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
