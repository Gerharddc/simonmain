import QtQuick 2.3
import "qrc:/Controls"

BottomPage {
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
