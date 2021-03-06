import QtQuick 2.6
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0

Button {
    id: buttonMenu
    signal clicked()
    width: 170
    height: 60
    property string iconPath_
    property bool selected_: false

    Image {
        id: form
        x:parent.width-42
        y:14
        fillMode: Image.PreserveAspectFit
        source: iconPath_
        height:32
        ToolTip {
            parent: form
            text: buttonMenu.text
            visible: buttonMenu_mouseArea.containsMouse
            delay: 500
            timeout: 5000
        }
    }

    //Partie texte
    contentItem: Text {
        text: buttonMenu.text + Translation.tr
        font.pixelSize: 12
        color: custompalette.text
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        rightPadding: 47
        font.capitalization: Font.AllUppercase
    }

    //Fond + icon
    background: Rectangle {
        id: buttonMenu_rect
        color: (selected_ == true) ? custompalette.darkInput : custompalette.darkThird
        width:parent.width
        height:59
    }

    Rectangle {
        anchors.top: buttonMenu_rect.bottom
        width: 170
        height: 1
        color: custompalette.border
    }

    //Capture du curseur
    MouseArea {
        id: buttonMenu_mouseArea
        onClicked: buttonMenu.clicked()
        anchors.fill: parent
        hoverEnabled: true
    }

    states: [
        State {
            name: "hover"
            when: buttonMenu_mouseArea.containsMouse && !buttonMenu_mouseArea.pressed
            PropertyChanges {
                target: buttonMenu_rect
                color : custompalette.border
            }
        },
        State {
            name: "pressed"
            when: buttonMenu_mouseArea.pressed
            PropertyChanges {
                target: buttonMenu_rect
                color : custompalette.darkInput
            }
        },
        State {
            name: "exit"
            when: !buttonMenu_mouseArea.containsMouse
            PropertyChanges {
                target: buttonMenu_rect
                color: (selected_ == true) ? custompalette.darkInput : custompalette.darkThird
            }
        }
    ]

    transitions: Transition {
        ColorAnimation { duration:100 }
    }
}
