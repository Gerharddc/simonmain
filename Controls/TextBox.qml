import QtQuick 2.3
import "qrc:/StyleSheet.js" as Style

DimmableControl {
    id: textRect
    color: Style.bgRed
    border.color: Style.accentColor
    border.width: 2

    property alias fontSize: _textInput.font.pixelSize
    property alias text: _textInput.text

    // Default
    width: 200
    height: fontSize + 20

    isActive: _textInput.focus

    MouseArea {
        anchors.fill: parent
        onClicked: {
            _textInput.focus = true;
        }

        TextInput {
            id: _textInput
            anchors.fill: parent
            anchors.margins: 10
            font.family: 'Nevis'
            font.pixelSize: 16
            color: Style.textColor
            clip: true

            onFocusChanged: {
                if (focus) {
                    keyboard.requestOpen(textRect)
                }
                else {
                    keyboard.requestClose(textRect)
                }
            }
        }
    }
}

