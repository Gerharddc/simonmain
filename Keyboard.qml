import QtQuick 2.3
import 'StyleSheet.js' as Style
import "qrc:/Controls"

Rectangle {
    id: keyboardRect
    z: 100

    // Default
    width: 480
    height: 350

    color: 'black'

    readonly property int maxRows: 10
    readonly property int columnCount: 4
    readonly property int keySpacing: 3
    readonly property int buttonWidth: ((keysColumn.width) / maxRows) - keySpacing //keysColumn.width / maxColumns
    readonly property int buttonHeight: ((keysColumn.height) / columnCount) - keySpacing //(keysColumn.height / columnCount)

    Component {
        id: keyDelegate

        Button {
            text: key
            width: buttonWidth
            height: buttonHeight
            fontSize: 20
        }
    }

    readonly property var rowIds: [firstRowModel, secondRowModel, thirdRowModel, fourthRowModel]

    ListModel {
        id: firstRowModel
        ListElement { key: "q" }
        ListElement { key: "w" }
        ListElement { key: "e" }
        ListElement { key: "r" }
        ListElement { key: "t" }
        ListElement { key: "y" }
        ListElement { key: "u" }
        ListElement { key: "i" }
        ListElement { key: "o" }
        ListElement { key: "p" }
    }

    ListModel {
        id: secondRowModel
        ListElement { key: "a" }
        ListElement { key: "s" }
        ListElement { key: "d" }
        ListElement { key: "f" }
        ListElement { key: "g" }
        ListElement { key: "h" }
        ListElement { key: "j" }
        ListElement { key: "k" }
        ListElement { key: "l" }
    }

    ListModel {
        id: thirdRowModel
        ListElement { key: "z" }
        ListElement { key: "x" }
        ListElement { key: "c" }
        ListElement { key: "v" }
        ListElement { key: "b" }
        ListElement { key: "n" }
        ListElement { key: "n" }
        ListElement { key: "m" }
    }

    ListModel {
        id: fourthRowModel
        ListElement { key: "Sft" }
        ListElement { key: "Spc" }
        ListElement { key: "Lft" }
        ListElement { key: "Rht" }
    }

    Component {
        id: rowDelegate

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: keySpacing

            Repeater {
                model: rowIds[rowId]
                delegate: keyDelegate
            }
        }
    }

    ListModel {
        id: alphabetSetModel
        ListElement { rowId: 0 }
        ListElement { rowId: 1 }
        ListElement { rowId: 2 }
        ListElement { rowId: 3 }
    }

    Rectangle {
        id: rectTop
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 5
        color: Style.accentColor
    }

    Column {
        id: keysColumn
        spacing: keySpacing
        anchors.top: rectTop.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: keySpacing
        anchors.topMargin: 2 * keySpacing // We need to compensate for the extra spacing caused at the bottom by the column

        Repeater {
            model: alphabetSetModel
            delegate: rowDelegate
        }
    }
}

