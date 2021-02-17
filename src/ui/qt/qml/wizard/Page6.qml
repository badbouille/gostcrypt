import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import "../" as UI

Item {
    id: top
    property variant sizeType: [0, "MB"]
    property int type: 0

    Row {
        id: containerSize
        spacing: 20
        anchors.horizontalCenter: parent.horizontalCenter
        Column {
            visible: (type === 2 || type === 3) ? false : true
            spacing: 10
            width: parent.width/2 - 5
            Text {
                id:titre
                font.pointSize: 13
                anchors.horizontalCenter: parent.horizontalCenter
                font.family: "Helvetica"
                text: qsTr("Set a size for your volume") + Translation.tr
                color: custompalette.text
                wrapMode: Text.WordWrap
            }

            TextField {
                id: sizeValue
                width: 250
                horizontalAlignment: TextInput.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
                height: 40
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator{}
                focus: true
                text: (volumeInfos.VOLUME_SIZE[0] !== 0)? volumeInfos.VOLUME_SIZE[0].toString() : ""
                style: TextFieldStyle {
                    textColor: "#e1e1e1"
                    background: Rectangle {
                        radius: 2
                        implicitWidth: 100
                        implicitHeight: 24
                        border.color: "#333"
                        border.width: 1
                        color: custompalette.darkInput
                    }
                }
                onTextChanged: {
                    volumeInfos.VOLUME_SIZE[0] = parseInt(sizeValue.text)
                    if (volumeInfos.VOLUME_SIZE[0] > 0) {
                        next.visible = true
                    } else {
                        next.visible = false
                    }
                }
                //Keys.onReturnPressed: manageWizard(1)
                //Keys.onEnterPressed: manageWizard(1)
            }

            Row {
                id: row
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 10
                ExclusiveGroup { id: groupRadio }
                UI.CheckBox {
                    id: kB_
                    text_: qsTr("KB")
                    checked: (volumeInfos.VOLUME_SIZE[1] == "KB")? true : false
                    height: 40
                    exclusiveGroup: groupRadio
                    onCheckedChanged: {
                        if(kB_.checked) volumeInfos.VOLUME_SIZE[1] = "KB";
                    }
                }
                UI.CheckBox {
                    id: mB_
                    text_: qsTr("MB")
                    checked: (volumeInfos.VOLUME_SIZE[1] == "MB")? true : false
                    height: 40
                    exclusiveGroup: groupRadio
                    onCheckedChanged: {
                        if(mB_.checked) volumeInfos.VOLUME_SIZE[1] = "MB";
                    }
                }
                UI.CheckBox {
                    id: gB_
                    text_: qsTr("GB")
                    checked: (volumeInfos.VOLUME_SIZE[1] == "GB")? true : false
                    height: 40
                    exclusiveGroup: groupRadio
                    onCheckedChanged: {
                        if(gB_.checked) volumeInfos.VOLUME_SIZE[1] = "GB";
                    }
                }
                UI.CheckBox {
                    id: tB_
                    text_: qsTr("TB")
                    checked: (volumeInfos.VOLUME_SIZE[1] == "TB")? true : false
                    height: 40
                    exclusiveGroup: groupRadio
                    onCheckedChanged: {
                        if(tB_.checked) volumeInfos.VOLUME_SIZE[1] = "TB";
                    }
                }
            }
        }

    }


    Text {
        id:description
        width: parent.width - 40
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: containerSize.bottom
        anchors.topMargin: 10
        font.pixelSize: 12
        text: qsTr("Please specify the size of the container you want to create.<br>If"
                  +" you create a dynamic (sparse-file) container, this parameter will specify its maximum possible size."
                  +"<br>Note that the minimal size for an ext2 volume is around 100KB.")
        y: 150
        x: 60
        color: custompalette.text
        horizontalAlignment: Text.AlignJustify
        wrapMode: Text.WordWrap
    }

    Text {
        id: freeDiskSpace
        text: ""
        wrapMode: Text.WordWrap
        x: bottomBar.x
        y: bottomBar.y-2
        width: bottomBar.width- 40
        height: bottomBar.height
        horizontalAlignment: Text.AlignHCenter
        color: custompalette.textLowOpacity
        font.pointSize: 9
    }

    function updateAvailableSpace(size) {
        setText(qsTr("<b>Free space on drive : ")+size)
    }

    function setText(bold) {
        freeDiskSpace.text = bold
    }
}
