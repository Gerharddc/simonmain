import QtQuick 2.3
import 'StyleSheet.js' as Style
import "qrc:/Controls"

Rectangle {
    id: keyboardRect
    z: 100

    // Default
    width: 480
    height: 350

    color: "#80000000"

    readonly property int maxRows: 10
    readonly property int columnCount: 5
    readonly property int keySpacing: 3
    readonly property int buttonWidth: ((keysColumn.width) / maxRows) - keySpacing
    readonly property int buttonHeight: ((keysColumn.height) / columnCount) - keySpacing
    property bool shifted: false
    //property bool open: keyboard.open

    /*onOpenChanged: {
        if (open) {
            console.log('opened')
        }
        else {
            console.log('closed')
        }
    }*/

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
        // TODO: implement hiding
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

    readonly property var functionIds: [charPressed, shiftPressed, hidePressed, spacePressed, enterPressed, leftPressed, rightPressed]

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
        ListElement { key: "Shift"; functionId: 1; widthMulti: 2 }
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
        ListElement { key: "Hide"; functionId: 2; widthMulti: 1.75 }
        ListElement { key: "."; widthMulti: 0.75 }
        ListElement { key: "<"; functionId: 5; widthMulti: 0.75 }
        ListElement { key: "Space"; functionId: 3; widthMulti: 4 }
        ListElement { key: ">"; functionId: 6; widthMulti: 0.75 }
        ListElement { key: "Enter"; functionId: 4; widthMulti: 2 }
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

