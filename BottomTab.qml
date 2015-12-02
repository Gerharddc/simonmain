import QtQuick 2.5
import 'StyleSheet.js' as Style

Rectangle {
    id: tabRect

    property alias tabText: tabTextBlock.text
    property int tabNum: 0

    signal tabClicked()

    color: mouseArea.pressed ? Style.pressedRed : 'transparent'

    Text {
        z: 10
        id: tabTextBlock
        anchors.centerIn: parent
        text: 'Model'
        font.family: 'Nevis'
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            tabRect.tabClicked()
        }
    }
}

