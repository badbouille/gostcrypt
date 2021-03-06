import QtQuick 2.0

Rectangle {
    id:rect_
    property string text_
    signal clicked()
    height: 40
    color: "transparent"
    width: parent.width
    Text {
        anchors.centerIn: parent
        text: text_ + Translation.tr
        color: custompalette.text
        font.pixelSize: 12
    }
    MouseArea {
        id: area
        anchors.fill: rect_
        hoverEnabled: true
        cursorShape: area.containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked: rect_.clicked()
    }

    states: [
        State {
            name: "hover"
            when: area.containsMouse && !area.pressed
            PropertyChanges {
                target: rect_
                color : custompalette.blue
            }
        },
        State {
            name: "pressed"
            when: area.pressed
            PropertyChanges {
                target: rect_
                color : "transparent"
            }
        },
        State {
            name: "exit"
            when: !area.containsMouse
            PropertyChanges {
                target: rect_
                color : "transparent"
            }
        }
    ]

    transitions: Transition {
        ColorAnimation { duration:100 }
    }
}
