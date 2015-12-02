import QtQuick 2.5
import 'StyleSheet.js' as Style
import "qrc:/Controls"

Rectangle {
    readonly property int maxRows: 10
    readonly property int columnCount: 5
    readonly property int keySpacing: 3
    readonly property int buttonWidth: ((keysColumn.width) / maxRows) - keySpacing
    readonly property int buttonHeight: ((keysColumn.height) / columnCount) - keySpacing
    property bool shifted: false

    z: 100
    width: 480 //default
    height: keyboard.open ? keyboard.keyboardHeight : 0
    opacity: keyboard.open ? 1 : 0.5
    visible: (height != 0)
    clip: true

    anchors.left: parent.left
    anchors.right: parent.right

    color: "#80000000"

    Behavior on height {
        PropertyAnimation { duration: 500 }
    }

    Behavior on opacity {
        PropertyAnimation { duration: 500 }
    }

    Rectangle {
        id: rectTop
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 5
        color: Style.accentColor
        z: 101
    }

    Component {
        id: keyDelegate

        Button {
            id: _btn
            text: shifted ? key.toUpperCase() : key
            width: widthMulti ? buttonWidth * widthMulti : buttonWidth
            height: buttonHeight
            fontSize: 20
            onClicked: {
                functionIds[functionId](_btn)
            }
        }
    }

    readonly property var rowIds: [numRowModel, firstRowModel, secondRowModel, thirdRowModel, fourthRowModel]

    function charPressed(btn) {
        keyboard.pressKey(btn.text)
    }

    function shiftPressed(btn) {
        shifted = !shifted
    }

    function hidePressed(btn) {
        keyboard.forceClose()
    }

    function spacePressed(btn) {
        keyboard.pressSpace()
    }

    function enterPressed(btn) {
        keyboard.pressEnter()
    }

    function leftPressed(btn) {
        keyboard.pressLeft()
    }

    function rightPressed(btn) {
        keyboard.pressRight()
    }

    function backspacePressed(btn) {
        keyboard.pressBackspace()
    }

    readonly property var functionIds: [charPressed, shiftPressed, hidePressed, spacePressed, enterPressed, leftPressed, rightPressed, backspacePressed]

    ListModel {
        id: numRowModel
        ListElement { key: "1"; functionId: 0; widthMulti: 1 }
        ListElement { key: "2" }
        ListElement { key: "3" }
        ListElement { key: "4" }
        ListElement { key: "5" }
        ListElement { key: "6" }
        ListElement { key: "7" }
        ListElement { key: "8" }
        ListElement { key: "9" }
        ListElement { key: "0" }
    }

    ListModel {
        id: firstRowModel
        ListElement { key: "q"; functionId: 0; widthMulti: 1 }
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
        ListElement { key: "a"; functionId: 0; widthMulti: 1 }
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
        ListElement { key: "⇑"; functionId: 1; widthMulti: 1 }
        ListElement { key: "z" }
        ListElement { key: "x" }
        ListElement { key: "c" }
        ListElement { key: "v" }
        ListElement { key: "b" }
        ListElement { key: "n" }
        ListElement { key: "n" }
        ListElement { key: "m" }
        ListElement { key: "⇐"; functionId: 7 }
    }

    ListModel {
        id: fourthRowModel
        ListElement { key: "⬇"; functionId: 2; widthMulti: 1 }
        ListElement { key: "."; widthMulti: 0.75 }
        ListElement { key: "◀"; functionId: 5; widthMulti: 1 }
        ListElement { key: "Space"; functionId: 3; widthMulti: 4.35 }
        ListElement { key: "▶"; functionId: 6; widthMulti: 1 }
        ListElement { key: "⏎"; functionId: 4; widthMulti: 1.75 }
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
        ListElement { rowId: 4 }
    }

    Column {
        id: keysColumn
        spacing: keySpacing
        height: keyboard.keyboardHeight - (5 * keySpacing) // 5 seems to be the magic number (not sure if because column count)
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: keySpacing

        Repeater {
            model: alphabetSetModel
            delegate: rowDelegate
        }
    }
}
