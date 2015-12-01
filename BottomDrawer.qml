import QtQuick 2.3
import QtQuick.Layouts 1.2
import "StyleSheet.js" as Style

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

        opacity: isExpanded ? 0.5 : 1

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

    RowLayout {
        id: tabBar

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
            activeNum: activeTabNum
            Layout.fillWidth: true
            Layout.fillHeight: true
            onTabClicked: enableTab(modelTab)
        }

        BottomTab {
            id: sliceTab
            tabText: 'Slice'
            tabNum: 1
            activeNum: activeTabNum
            Layout.fillWidth: true
            Layout.fillHeight: true
            onTabClicked: enableTab(sliceTab)
        }

        BottomTab {
            id: printTab
            tabText: 'Print'
            tabNum: 2
            activeNum: activeTabNum
            Layout.fillWidth: true
            Layout.fillHeight: true
            onTabClicked: enableTab(printTab)
        }
    }

    BottomPages {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: (iExpandedHeight - tabBar.height)

        pageNum: activeTabNum

    }
}

