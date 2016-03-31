import QtQuick 2.3
import "qrc:/Controls"
import "qrc:/StyleSheet.js" as Style

BottomPage {
    Item {
        anchors.fill: parent
        anchors.margins: 20        

        Button {
            id: btnAdd
            text: "Add model"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 5
            isDimmable: true

            onClicked: {
                rootRect.addStl()
            }
        }

        Label {
            id: txtSelect
            text: 'Select some models to see more options'
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: btnAdd.bottom
            anchors.topMargin: 15
            isDimmable: true
            visible: renderer.meshesSelected === 0
        }

        Button {
            id: btnRemove
            text: "Remove selected models"
            anchors.top: btnAdd.bottom
            anchors.topMargin: 15
            anchors.left: parent.left
            anchors.right: parent.right
            isDimmable: true
            visible: renderer.meshesSelected > 0

            onClicked: {
                renderer.removeSelectedMeshes()
            }
        }

        Label {
            id: txtOne
            text: 'Select only one model to change its properties'
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: btnRemove.bottom
            anchors.topMargin: 15
            isDimmable: true
            visible: renderer.meshesSelected > 1
        }

        Item {
            anchors.top: btnRemove.bottom
            anchors.topMargin: 15
            anchors.left: parent.left
            anchors.right: parent.right

            visible: renderer.meshesSelected === 1

            Label {
                id: txtPos
                text: 'Selected model properties'
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                isDimmable: true
            }

            Row {
                id: rowPos
                spacing: 10
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: txtPos.bottom
                anchors.topMargin: 15

                Item {
                    width: (parent.width - 20) / 3
                    height: childrenRect.height

                    Label {
                        id: txtX
                        text: 'X Pos:'
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        isDimmable: true
                    }

                    TextBox {
                        id: tboxX
                        anchors.top: txtX.bottom
                        anchors.topMargin: 5
                        anchors.left: parent.left
                        anchors.right: parent.right
                        isDimmable: true

                        Binding on text {
                            when: !tboxX.isActive && !curMeshPosAnimation.running
                            value: renderer.curMeshPos.x.toFixed(2).replace(/\.?0+$/, "")
                        }
                    }

                    Binding {
                        target: renderer
                        property: "curMeshPos.x"
                        value: tboxX.text
                    }
                }

                Item {
                    width: (parent.width - 20) / 3
                    height: childrenRect.height

                    Label {
                        id: txtY
                        text: 'Y Pos:'
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        isDimmable: true
                    }

                    TextBox {
                        id: tboxY
                        anchors.top: txtY.bottom
                        anchors.topMargin: 5
                        anchors.left: parent.left
                        anchors.right: parent.right
                        isDimmable: true

                        Binding on text {
                            when: !tboxY.isActive && !curMeshPosAnimation.running
                            value: renderer.curMeshPos.y.toFixed(2).replace(/\.?0+$/, "")
                        }
                    }

                    Binding {
                        target: renderer
                        property: "curMeshPos.y"
                        value: tboxY.text
                    }
                }

                Item {
                    width: (parent.width - 20) / 3
                    height: childrenRect.height

                    Label {
                        id: txtS
                        text: 'Scale:'
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        isDimmable: true
                    }

                    TextBox {
                        id: tboxS
                        anchors.top: txtS.bottom
                        anchors.topMargin: 5
                        anchors.left: parent.left
                        anchors.right: parent.right
                        isDimmable: true

                        Binding on text {
                            when: !tboxS.isActive && !curMeshScaleAnimation.running
                            value: renderer.curMeshScale
                        }
                    }

                    Binding {
                        target: renderer
                        property: "curMeshScale"
                        value: tboxS.text
                    }
                }
            }

            Column {
                spacing: 10
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: rowPos.bottom
                anchors.topMargin: 10

                Item {
                    width: parent.width
                    height: childrenRect.height

                    Label {
                        id: lblX
                        text: 'X Pos:'
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        isDimmable: true
                    }

                    Slider {
                        id: sldrX
                        anchors.top: lblX.bottom
                        anchors.topMargin: 10
                        width: parent.width
                        height: 50
                        isDimmable: true
                        value: renderer.curMeshRot.x
                    }

                    Binding {
                        target: renderer
                        property: "curMeshRot.x"
                        value: sldrX.value
                    }
                }

                Item {
                    width: parent.width
                    height: childrenRect.height

                    Label {
                        id: lblY
                        text: 'Y Pos:'
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        isDimmable: true
                    }

                    Slider {
                        id: sldrY
                        anchors.top: lblY.bottom
                        anchors.topMargin: 10
                        width: parent.width
                        height: 50
                        isDimmable: true
                        value: renderer.curMeshRot.y
                    }

                    Binding {
                        target: renderer
                        property: "curMeshRot.y"
                        value: sldrY.value
                    }
                }

                Item {
                    width: parent.width
                    height: childrenRect.height

                    Label {
                        id: lblZ
                        text: 'Z Pos:'
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        isDimmable: true
                    }

                    Slider {
                        id: sldrZ
                        anchors.top: lblZ.bottom
                        anchors.topMargin: 10
                        width: parent.width
                        height: 50
                        isDimmable: true
                        value: renderer.curMeshRot.z

                        Binding on value {
                            when: !sldrZ.isActive
                            value: renderer.curMeshRot.z
                        }
                    }

                    Binding {
                        target: renderer
                        property: "curMeshRot.z"
                        value: sldrZ.value
                    }
                }
            }
        }

        /*DropTextBox {
            anchors.top: btn1.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 15
            isDimmable: true
            options: ListModel {
                            ListElement { option: "Een" }
                            ListElement { option: "Twee" }
                            ListElement { option: "Drie" }
                            ListElement { option: "Vier" }
                            ListElement { option: "Vyf" }
                        }
        }*/
    }
}

