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
                text: qsTr("Volume Type") + Translation.tr
            }

            UI.CustomComboBox {
                id: selector
                model: app.volumetypes["id"]
                currentIndex: volumeInfos.VOLUME_TYPE
                anchors.top: text.bottom
                anchors.topMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width -20
                onActivated: {
                    volumeInfos.VOLUME_TYPE = selector.currentIndex
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
                text: app.volumetypes["name"][selector.currentIndex]
            }

            Text {
                id: description
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WordWrap
                width: parent.width + 20
                text: app.volumetypes["description"][selector.currentIndex];
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
