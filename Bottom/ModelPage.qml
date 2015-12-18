import QtQuick 2.3
import "qrc:/Controls"

BottomPage {
    TextBox {
        x: 100
        y: 100
        width: 200
        isDimmable: true
    }

    Button {
        width: 200
        text: "een"
        x: 100
        y: 200
        isDimmable: true
    }

    DropTextBox {
        x: 100
        y: 300
        width: 250
        isDimmable: true
        options: ListModel {
                        ListElement { option: "Een" }
                        ListElement { option: "Twee" }
                        ListElement { option: "Drie" }
                        ListElement { option: "Vier" }
                        ListElement { option: "Vyf" }
                    }
    }
}

