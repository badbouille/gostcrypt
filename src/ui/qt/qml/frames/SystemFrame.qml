import QtQuick 2.0

Item {
    property QtObject mainWindow_
    signal menuChanged(string name, int index)
    width: app.width

    BackgroundFrame {
        name: "System"
    }

    Text {
        id:title
        font.pixelSize: 25
        color: custompalette.text
        anchors.horizontalCenter: parent.horizontalCenter
        y: 20
        text: qsTr("System") + Translation.tr
    }
}
