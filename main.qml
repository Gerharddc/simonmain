import QtQuick 2.3

Item {
    visible: true
    width: 800
    height: 480

    MainScreen {
        width: parent.height
        height: parent.width
        rotation: 270
        anchors.centerIn: parent
        //anchors.fill: parent
        //x: 20
    }

    /*MultiPointTouchArea {
            anchors.fill: parent
            touchPoints: [
                TouchPoint { id: point1 },
                TouchPoint { id: point2 }
            ]
        }

        Rectangle {
            width: 30; height: 30
            color: "green"
            x: point1.x
            y: point1.y
        }

        Rectangle {
            width: 30; height: 30
            color: "yellow"
            x: point2.x
            y: point2.y
        }*/
}

