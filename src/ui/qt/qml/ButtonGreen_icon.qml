import QtQuick 2.6
import QtQuick.Controls 2.0

Button {
    id: buttonGreenIcon
    signal clicked()
    width: 160
    height: 37

    //Partie texte
    contentItem: Text {
        leftPadding: 37
        text: buttonGreenIcon.text + Translation.tr
        font.pixelSize: 13
        color: custompalette.textVeryLight
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    //Fond + icon
    background: Rectangle {
        id: buttonGreenIcon_rect
        Image {
            id: buttonGreenIcon_menuBars
            x: 0
            y: 0
            width: 37
            height: 37
            enabled: false
            smooth: false
            source: "ressource/buttonGreenLeft.png"
        }
        color: custompalette.dark
        implicitWidth: 160
        implicitHeight: 37
        border.width: 1
        radius: 2
        border.color: custompalette.green
    }

    //Capture du curseur
    MouseArea {
        id: buttonGreenIcon_mouseArea
        onClicked: buttonGreenIcon.clicked()
        anchors.fill: parent
        hoverEnabled: true
    }

    states: [
        State {
            name: "hover"
            when: buttonGreenIcon_mouseArea.containsMouse && !buttonGreenIcon_mouseArea.pressed
            PropertyChanges {
                target: buttonGreenIcon_rect
                color : custompalette.greenHover
            }
        },
        State {
            name: "pressed"
            when: buttonGreenIcon_mouseArea.pressed
            PropertyChanges {
                target: buttonGreenIcon_rect
                color : custompalette.greenDark
            }
        },
        State {
            name: "exit"
            when: !buttonGreenIcon_mouseArea.containsMouse
            PropertyChanges {
                target: buttonGreenIcon_rect
                color: custompalette.dark
            }
        }
    ]

    transitions: Transition {
        ColorAnimation { duration:100 }
    }
}
