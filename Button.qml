import QtQuick 2.3
import "qrc:/StyleSheet.js" as Style

Rectangle {
    id: rectMain
    color: Style.bgRed
    border.color: Style.accentColor
    border.width: 2

    // Default
    width: 100
    height: 50

    property alias text: _text.text
    property alias fontSize: _text.font.pixelSize

    signal clicked()
    signal pressed()
    signal canceled()
    signal released()

    Text {
        id: _text
        anchors.centerIn: parent
        font.family: 'Nevis'
        color: Style.textColor
        text: 'Button'
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            rectMain.clicked()
        }

        onPressed: {
            rectMain.pressed()
            rectMain.color = Style.pressedRed
        }

        onCanceled: {
            rectMain.canceled()
            rectMain.color = Style.bgRed
        }

        onReleased: {
            rectMain.released()
            rectMain.color = Style.bgRed
        }
    }
}

