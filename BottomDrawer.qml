import QtQuick 2.3
import "StyleSheet.js" as Style

Item {
    id: theBottomDrawer


    readonly property int iClosedHeight: 50;
    readonly property int iExpandedHeight: 700;

    width: 480 // default
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
        id: bottomBG
        color: Style.bgRed
        anchors.fill: parent

        NumberAnimation on opacity {
            id: anim_Transparent
            from: 1.0
            to: 0.5
            loops: 1
            running: false
        }

        NumberAnimation on opacity {
            id: anim_Opaque
            from: 0.5
            to: 1.0
            loops: 1
            running: false
        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 5
        color:  Style.accentColor
    }

    Image {
        id: img_Expander
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 10
        anchors.rightMargin: 10
        width: 35
        height: 35
        source: "Images/Chevron Down-50.png"
        rotation: 180

        RotationAnimation on rotation {
            id: anim_Open
            loops: 1
            from: 180
            to: 0
            running: false
        }

        RotationAnimation on rotation {
            id: anim_Close
            loops: 1
            from: 0
            to: 180
            running: false
        }

        MouseArea {
            anchors.fill: parent

            onPressed: {
                if (!theBottomDrawer.isExpanded) {
                    anim_Open.start()
                }
                else {
                    anim_Close.start()
                }
            }

            onReleased: {
                if (!theBottomDrawer.isExpanded) {
                    anim_Expand.start()
                    anim_Transparent.start()
                }
                else {
                    anim_Contract.start()
                    anim_Opaque.start()
                }

                theBottomDrawer.isExpanded = !theBottomDrawer.isExpanded
            }

            onCanceled: {
                if (theBottomDrawer.isExpanded) {
                    anim_Open.start()
                }
                else {
                    anim_Close.start()
                }
            }
        }
    }

}

