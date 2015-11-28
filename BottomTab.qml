import QtQuick 2.3
import 'StyleSheet.js' as Style

Rectangle {
    id: tabRect

    property alias tabText: tabTextBlock.text
    property Item enabledTab
    property bool tabEnabled: (enabledTab == tabRect)

    signal tabClicked()

    color: mouseArea.pressed ? Style.pressedRed : (tabEnabled ? Style.bgRed : 'transparent')

    Text {
        id: tabTextBlock
        anchors.centerIn: parent
        text: 'Model'
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            tabRect.tabClicked()
        }
    }
}

