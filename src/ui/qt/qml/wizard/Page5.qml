import QtQuick 2.7
import "../" as UI

Item {
    id: top

    Row {
        id: content
        spacing: 20
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 20
        Item {
            width: 250
            height: 150
            Text {
                id: text
                anchors.horizontalCenter: parent.horizontalCenter
                y:5
                font.pointSize: 11
                font.family: "Helvetica"
                color: custompalette.text
                text: qsTr("FUSE File System") + Translation.tr
            }

            UI.CustomComboBox {
                id: selector
                model: app.filesystems["id"]
                currentIndex: volumeInfos.VOLUME_FS
                anchors.top: text.bottom
                anchors.topMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width -20
                onActivated: {
                    volumeInfos.VOLUME_FS = selector.currentIndex
                }

            }

            Text {
                id: link
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WordWrap
                width: parent.width - 20
                text: qsTr("<font color='#719c24'><a href='http://en.wikipedia.org/wiki/Filesystem_in_Userspace'>Get more informations on FUSE</a></font>") + Translation.tr
                color: custompalette.text
                font.family: "Helvetica"
                anchors.top: selector.bottom
                anchors.topMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 12
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }

        }
        Item {
            width: 250
            height: 150

            Text {
                id: text2
                anchors.horizontalCenter: parent.horizontalCenter
                y:5
                font.pointSize: 11
                font.family: "Helvetica"
                color: custompalette.text
                text: app.filesystems["name"][selector.currentIndex]
            }

            Text {
                id: description
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WordWrap
                width: parent.width + 20
                text: app.filesystems["description"][selector.currentIndex];
                color: custompalette.text
                font.pointSize: 9
                font.family: "Helvetica"
                anchors.top: text2.bottom
                anchors.topMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }


        }
    }
}
