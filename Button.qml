import QtQuick 2.3
import "qrc:/StyleSheet.js" as Style

Rectangle {
    color: Style.bgRed
    border.color: Style.accentColor
    border.width: 2

    // Default
    width: 100
    height: 50

    property alias Text: _text.text

    Text {
        id: _text
        anchors.centerIn: parent
        font.family: 'Nevis'
        color: Style.textColor
        text: 'Button'
    }
}

