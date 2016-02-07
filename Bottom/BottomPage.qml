import QtQuick 2.3

Flickable {
    id: modelPage
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    width: 100 // Default
    clip: true
    contentWidth: width
    contentHeight: height + contentHeightPlus

    property int contentHeightPlus: 0
}
