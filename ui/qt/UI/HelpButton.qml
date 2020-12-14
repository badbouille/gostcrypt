import QtQuick 2.0

Rectangle {
    id: helpButton
    property int size: 20
    signal clicked()

    width: size
    height: size
    radius: size/2
    color: custompalette.bkCheckBox
    Text {
        anchors.centerIn: parent
        text: qsTr("<b>?</b>") + Translation.tr
        font.pixelSize: size*0.6
        color: custompalette.green
    }
    MouseArea {
        id: helpMouseArea
        onClicked: helpButton.clicked()
        anchors.fill: parent
        hoverEnabled: true
    }
}
