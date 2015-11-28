import QtQuick 2.3

Rectangle {
    id: tabRect

    property alias tabText: tabTextBlock.text
    property color tabColor
    property Item enabledTab
    property bool tabEnabled: (enabledTab == tabRect)

    signal tabClicked()

    color: tabEnabled ? 'transparent' : tabColor

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

