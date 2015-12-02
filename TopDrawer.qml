import QtQuick 2.5
import "StyleSheet.js" as Style

Rectangle {
    id: theTopDrawer
    color: Style.bgRed
    clip: true

    readonly property int iExpandedHeight: 400
    readonly property int iClosedHeight: 50

    width: 480 // Default
    height: isExpanded ? iExpandedHeight : iClosedHeight

    property bool isExpanded: false

    Behavior on height {
        PropertyAnimation {}
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 5
        color:  Style.accentColor
    }

    Image {
        id: img_Expander
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.bottomMargin: 10
        anchors.rightMargin: 10
        width: 35
        height: 35
        source: "Images/Chevron Down-50.png"
        rotation: isExpanded ? (flipMouse.pressed ? 0 : 180) : (flipMouse.pressed ? 180 : 0)

        Behavior on rotation {
            PropertyAnimation {}
        }

        MouseArea {
            id: flipMouse
            anchors.fill: parent

            onClicked: {
                theTopDrawer.isExpanded = !theTopDrawer.isExpanded
            }
        }
    }

    Item {
        id: hideableContent
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: iClosedHeight


    }
}

