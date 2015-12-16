import QtQuick 2.3
import "qrc:/StyleSheet.js" as Style

DimmableControl {
    id: dropper
    color: Style.bgRed
    border.color: Style.accentColor
    border.width: 2

    property int fontSize: 20
    property alias text: _textInput.text
    property bool isExpanded: false
    property int iExpandedHeight: 150

    property alias options: optionListView.model

    // Default
    width: 200
    height: fontSize + 30

    property bool buttonActive: false
    isActive: _textInput.focus || isExpanded || buttonActive
    focus: flipMouse.pressed || isExpanded

    onIsActiveChanged: {
        if (!isActive)
            isExpanded = false
    }

    MouseArea {
        anchors.left: parent.left
        anchors.right: rightRect.left
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        z: 10

        onClicked: {
            _textInput.focus = true
        }
    }

    TextInput {
        id: _textInput
        anchors.verticalCenter: parent.verticalCenter
        x: 15
        width: parent.width - rightRect.width - 30
        anchors.margins: 15
        font.family: 'Nevis'
        color: Style.textColor
        font.pixelSize: fontSize
        clip: true
        z: 50

        onFocusChanged: {
            if (focus) {
                keyboard.requestOpen(dropper)
                isExpanded = false // We dont want the keyboard open with the dropdown menu
            }
            else {
                keyboard.requestClose(dropper)
            }
        }
    }

    Rectangle {
        id: dropRect
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: rightRect.left
        anchors.rightMargin: -2
        height: isExpanded ? iExpandedHeight : dropper.height
        color: Style.bgRed
        border.width: 2
        border.color: Style.accentColor
        z: -10

        Behavior on height {
            PropertyAnimation {}
        }

        ListModel {
            id: optionModel
            ListElement { option: "Alice" }
            ListElement { option: "Bob" }
            ListElement { option: "Jane" }
            ListElement { option: "Harry" }
            ListElement { option: "Wendy" }
        }

        Component {
            id: optionDelegate

            Rectangle {
                id: optionRect
                height: fontSize + 8
                width: parent.width
                color: 'transparent'

                Text {
                    anchors.fill: parent
                    text: option;
                    font.pixelSize: fontSize
                    color: Style.textColor
                    font.family: 'Nevis'
                }

                MouseArea {
                    id: _mouseArea
                    anchors.fill: parent

                    onPressed: {
                        optionRect.color = Style.pressedRed
                    }

                    onReleased: {
                        optionRect.color = 'transparent'
                        _textInput.text = option
                        buttonActive = false
                    }

                    onCanceled: {
                        optionRect.color = 'transparent'
                        buttonActive = false
                    }

                    onPressAndHold: {
                        buttonActive = true
                    }
                }
            }
        }

        ListView {
            id: optionListView
            anchors.fill: parent
            anchors.topMargin: dropper.height + 5
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            anchors.bottomMargin: 5
            clip: true
            model: optionModel
            delegate: optionDelegate
        }
    }

    Rectangle {
        id: rightRect
        border.width: 2
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: height
        border.color: Style.accentColor
        color: 'transparent'

        Image {
            id: img_Dropdown
            source: "qrc:/Images/Chevron Down-50.png"
            anchors.fill: parent
            anchors.margins: 5

            rotation: isExpanded ? (flipMouse.pressed ? 0 : 180) : (flipMouse.pressed ? 180 : 0)

            Behavior on rotation {
                PropertyAnimation {}
            }

            MouseArea {
                id: flipMouse
                anchors.fill: parent

                onClicked: {
                    dropper.isExpanded = !dropper.isExpanded
                }

                onReleased: {
                    buttonActive = false
                }

                onCanceled: {
                    buttonActive = false
                }

                onPressAndHold: {
                    buttonActive = true
                }
            }
        }
    }
}

