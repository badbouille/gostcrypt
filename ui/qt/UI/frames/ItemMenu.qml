import QtQuick 2.7
import QtGraphicalEffects 1.0

Item {
    property string text_: ""
    property string description_: ""
    width: 128
    height: 90

    RectangularGlow {
        id: effect
        anchors.fill: rect_
        glowRadius: 10
        spread: 0.2
        color: custompalette.darkThird
        cornerRadius: rect_.radius + glowRadius
    }
    Rectangle {
        id:rect_
        anchors.fill:parent
        color: custompalette.dark
        radius:5
        border.color: custompalette.darkThird
        border.width: 1
        ColorAnimation on color { from: custompalette.darkThird; duration: 1000 }
    }
    Rectangle {
        id: textAlign
        color: "transparent"
        x:0
        y:0
        height: 55
        width: 128
    }
    Rectangle {
        id: descriptionAlign
        color: custompalette.hoverItemMenu
        x:0
        y:55
        height: 35
        width: 128
        border.color: custompalette.darkThird
        border.width: 1
    }

    Text {
        text: text_ + Translation.tr
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: textAlign.verticalCenter
        lineHeight: 0.75
        horizontalAlignment: Text.AlignHCenter
        color: custompalette.text
        font.pixelSize: 13
        width: 110
        wrapMode: Text.WordWrap
    }
    Text {
        text: description_ + Translation.tr
        color: custompalette.darkInput
        font.pixelSize: 11
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: descriptionAlign.verticalCenter
        horizontalAlignment: Text.AlignHCenter
        width: 120
        wrapMode: Text.WordWrap
    }

    MouseArea {
        id: area
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: area.containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
    }

    states: [
        State {
            name: "hover"
            when: area.containsMouse && !area.pressed
            PropertyChanges {
                target: rect_
                color : custompalette.hoverItemMenu
            }
        },
        State {
            name: "pressed"
            when: area.pressed
            PropertyChanges {
                target: rect_
                color : custompalette.dark
            }
        },
        State {
            name: "exit"
            when: !area.containsMouse
            PropertyChanges {
                target: rect_
                color : custompalette.dark
            }
        }
    ]

    transitions: Transition {
        ColorAnimation { duration:100 }
    }
}
