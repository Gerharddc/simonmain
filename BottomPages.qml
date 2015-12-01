import QtQuick 2.3
import "qrc:/Controls"

Item {
    property int pageNum: 0

    property alias pageWidth: referencePage.width
    property int pageHeight: referencePage.height

    Item {
        id: referencePage
        anchors.fill: parent
    }

    Item {
        id: pageContainer

        width: 3 * pageWidth
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.leftMargin: -(pageNum * pageWidth)

        Item {
            id: modelPage
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            width: pageWidth

            Button {
                width: 300
                height: 70
                text: "een"
                anchors.centerIn: parent
            }
        }

        Item {
            id: slicePage
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: modelPage.left
            width: pageWidth
        }

        Item {
            id: printPage
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: slicePage.left
            width: pageWidth
        }
    }
}

