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
                text: qsTr("Encryption Algorithm") + Translation.tr
            }

            UI.CustomComboBox {
                id: selector
                model: app.algorithms["id"]
                currentIndex: volumeInfos.VOLUME_ALGORITHM
                anchors.top: text.bottom
                anchors.topMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width -20
                onActivated: {
                    volumeInfos.VOLUME_ALGORITHM = selector.currentIndex
                }

            }
            Row {
                id: buttons
                spacing: 20
                anchors.top: selector.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 10

                UI.ButtonBordered {
                    id: testbutton
                    color_: custompalette.green
                    text: qsTr("Test") + Translation.tr
                    width: 70
                    height: 40
                    onClicked: {
                        //TODO
                    }
                }

                UI.ButtonBordered {
                    id: benchmarkButton
                    color_: custompalette.green
                    text: qsTr("Benchmark") + Translation.tr
                    width: 90
                    height: 40
                    onClicked: {
                        openSubWindow("dialogs/Benchmark.qml", qsTr("Benchmark"), qsTr("Benchmark"), 429, {"name" : "", "value" : ""})
                    }
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
                text: app.algorithms["name"][selector.currentIndex]
            }

            Text {
                id: description
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WordWrap
                width: parent.width + 20
                text: app.algorithms["description"][selector.currentIndex];
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
