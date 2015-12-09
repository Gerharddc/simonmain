import QtQuick 2.3
import "StyleSheet.js" as Style
import "DimmLogic.js" as DimmLogic

Item {
    id: theBottomDrawer
    z: 10
    clip: true
    
    readonly property int iClosedHeight: 50;
    readonly property int iExpandedHeight: (800 - iClosedHeight + 5);

    property bool isExpanded: false

    width: 480 // default
    height: isExpanded ? iExpandedHeight : iClosedHeight

    Behavior on height {
        PropertyAnimation {}
    }

    Rectangle {
        id: bottomBG
        color: Style.overlayGrey
        anchors.fill: parent

        opacity: isExpanded ? (DimmLogic.bindable.dimmInactives ? 0.2 : 0.5) : 1

        Behavior on opacity {
            PropertyAnimation {}
        }
    }

    Rectangle {
        id: rect_Chrome
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
        rotation: isExpanded ? (flipMouse.pressed ? 180 : 0) : (flipMouse.pressed ? 0 : 180)

        Behavior on rotation {
            PropertyAnimation {}
        }

        MouseArea {
            id: flipMouse
            anchors.fill: parent

            onClicked: {
                theBottomDrawer.isExpanded = !theBottomDrawer.isExpanded
            }
        }
    }

    property int activeTabNum: 0

    function enableTab(tabRef) {
        activeTabNum = tabRef.tabNum
    }

    Row {
        id: tabBar
        z: 10

        anchors.left:parent.left
        anchors.right: img_Expander.left
        anchors.top: rect_Chrome.bottom
        anchors.rightMargin: 10
        height: iClosedHeight - 5
        spacing: 0

        BottomTab {
            id: modelTab
            tabText: 'Model'
            tabNum: 0
            height: parent.height
            width: parent.width / 3
            onTabClicked: enableTab(modelTab)
        }

        BottomTab {
            id: sliceTab
            tabText: 'Slice'
            tabNum: 1
            height: parent.height
            width: parent.width / 3
            onTabClicked: enableTab(sliceTab)
        }

        BottomTab {
            id: printTab
            tabText: 'Print'
            tabNum: 2
            height: parent.height
            width: parent.width / 3
            onTabClicked: enableTab(printTab)
        }
    }

    Rectangle {
        id: tabSlider
        anchors.top: tabBar.top
        anchors.bottom: tabBar.bottom
        width: modelTab.width
        anchors.left: tabBar.left
        anchors.leftMargin: activeTabNum * modelTab.width
        color: Style.bgRed
        z: 5
        opacity: DimmLogic.hideInactives ? 0.5 : 1

        Behavior on anchors.leftMargin {
            PropertyAnimation {}
        }
    }

    BottomPages {
        id: bottomPages
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: (iExpandedHeight - tabBar.height)
        visible: isExpanded

        pageNum: activeTabNum

    }
}

