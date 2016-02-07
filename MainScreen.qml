import QtQuick 2.3
import "StyleSheet.js" as Style
import "qrc:/Controls"
import "qrc:/Keyboard"
import "qrc:/Bottom"
import FBORenderer 1.0

Rectangle {
    id: rootRect
    visible: true
    width: 480
    height: 800
    color: Style.bgMain

    TopDrawer {
        id: topDrawer
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        z: 100;
    }

    BottomDrawer {
        id: bottomDrawer
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        anchors.bottomMargin: keyboard.uiOffset

        Behavior on anchors.bottomMargin {
            PropertyAnimation { }
        }
    }

    Keyboard {
        objectName: "keyboard"
        anchors.left: parent.left
        anchors.bottom: parent.bottom
    }

    Renderer {
        id: renderer
        width: parent.width
        height: parent.height - bottomDrawer.iClosedHeight - topDrawer.iClosedHeight
        anchors.bottom: parent.bottom
        anchors.bottomMargin: bottomDrawer.isExpanded ? 0 : bottomDrawer.iClosedHeight
        Behavior on anchors.bottomMargin {
            PropertyAnimation {}
        }

        MouseArea {
            anchors.fill: parent

            MultiPointTouchArea {
                anchors.fill: parent
                mouseEnabled: false
            }

            property real lastX: 0
            property real lastY: 0
            property bool started: false
            property bool panning: false

            onPressed: {
                lastX = mouseX
                lastY = mouseY
                started = true
            }

            onReleased: {
                started = false
            }

            onPositionChanged: {
                var deltaX = mouseX - lastX
                var deltaY = mouseY - lastY

                if ((deltaX != 0) || (deltaY != 0))
                {
                    if (panning)
                        renderer.panView(deltaX, deltaY);
                    else
                        renderer.rotateView(deltaX, deltaY);
                }

                lastX = mouseX
                lastY = mouseY
            }

            onWheel: {
                if (wheel.angleDelta.y > 0)
                    renderer.zoomView(1.1)
                else if (wheel.angleDelta.y < 0)
                    renderer.zoomView(0.9)
            }

            Button {
                z: 10
                width: 100
                height: 40
                text: "Drag"
                anchors.left: parent.left
                anchors.top: parent.top

                onClicked: {
                    parent.panning = true
                }
            }

            Button {
                z: 10
                width: 100
                height: 40
                text: "Rotate"
                anchors.right: parent.right
                anchors.top: parent.top

                onClicked: {
                    parent.panning = false
                }
            }
        }
    }
}

