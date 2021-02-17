import QtQuick 2.7
import"../" as UI

Item {
    id: top
    anchors.fill: parent
    Text {
        id: minititle
        font.pointSize: 16
        text: qsTr("Parameters Summary")
        color: custompalette.text
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        wrapMode: Text.WordWrap
        width: 250
    }
    Row {
        id: content
        spacing: 20
        anchors.top: minititle.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 10
        Text {
            id:infos
            font.pointSize: 11
            text:    qsTr("Volume type: ") +
            "<br>" + qsTr("Volume path: ") +
            "<br>" + qsTr("Volume mountpoint: ") +
            "<br>" + qsTr("Volume algorithm: ") +
            "<br>" + qsTr("Volume KDF: ") +
            "<br>" + qsTr("Volume Filesystem: ") +
            "<br>" + qsTr("Volume size: ") +
            "<br>" + qsTr("Volume password: ")
            color: custompalette.text
            horizontalAlignment: Text.AlignLeft
            wrapMode: Text.WordWrap
            width: 150
        }
        Text {
            id:infos_val
            font.pointSize: 11
            text: "<b>" + app.volumetypes["name"][volumeInfos.VOLUME_TYPE] +
            "<br>" + volumeInfos.VOLUME_PATH +
            "<br>" + volumeInfos.VOLUME_MOUNTPOINT +
            "<br>" + app.algorithms["name"][volumeInfos.VOLUME_ALGORITHM] +
            "<br>" + app.hashs["name"][volumeInfos.VOLUME_HASH] +
            "<br>" + app.filesystems["name"][volumeInfos.VOLUME_FS] +
            "<br>" + volumeInfos.VOLUME_SIZE[0] + volumeInfos.VOLUME_SIZE[1] +
            "<br>" + volumeInfos.VOLUME_PWD.replace(/./g, "\u25CF")
            color: custompalette.text
            horizontalAlignment: Text.AlignRight
            wrapMode: Text.WordWrap
            width: 350
        }
    }
    UI.ButtonBordered {
        anchors.top: content.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 10
        text: qsTr("Create Volume")
        color_: custompalette.blue
        onClicked: {
            createVolume()
            catchClose()
        }
    }
}
