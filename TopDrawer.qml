import QtQuick 2.3
import "StyleSheet.js" as Style

Rectangle {
    id: theTopDrawer
    color: Style.bgRed
    clip: true

    readonly property int iExpandedHeight: 400
    readonly property int iClosedHeight: 50

    width: 480 // Default
    height: iClosedHeight

    property bool isExpanded: false

    NumberAnimation on height {
        id: anim_Expand
        from: iClosedHeight
        to: iExpandedHeight
        loops: 1
        running: false
    }

    NumberAnimation on height {
        id: anim_Contract
        from: iExpandedHeight
        to: iClosedHeight
        loops: 1
        running: false
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

        RotationAnimation on rotation {
            id: anim_Open
            loops: 1
            from: 0
            to: 180
            running: false
        }

        RotationAnimation on rotation {
            id: anim_Close
            loops: 1
            from: 180
            to: 0
            running: false
        }

        MouseArea {
            anchors.fill: parent

            onPressed: {
                if (!theTopDrawer.isExpanded) {
                    anim_Open.start()

                }
                else {
                    anim_Close.start()
                }
            }

            onReleased: {
                if (!theTopDrawer.isExpanded) {
                    anim_Expand.start()

                }
                else {
                    anim_Contract.start()
                }

                theTopDrawer.isExpanded = !theTopDrawer.isExpanded
            }

            onCanceled: {
                if (theTopDrawer.isExpanded) {
                    anim_Open.start()

                }
                else {
                    anim_Close.start()
                }
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

