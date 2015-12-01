import QtQuick 2.3
import 'StyleSheet.js' as Style

Rectangle {
    id: tabRect

    property alias tabText: tabTextBlock.text
    property int tabNum: 0
    property int activeNum: 0
    property bool tabEnabled: (activeNum == tabNum)

    signal tabClicked()

    color: mouseArea.pressed ? Style.pressedRed : (tabEnabled ? Style.bgRed : 'transparent')

    Text {
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

