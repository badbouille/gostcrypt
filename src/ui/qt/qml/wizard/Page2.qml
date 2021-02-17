import QtQuick 2.7
import QtQuick.Dialogs 1.2
import "../" as UI

Item {
    id: top
    property string path: ""
    property string message: qsTr("A GostCrypt volume can reside in a file (called GostCrypt container),"
                                    +" which can reside on a hard disk, on a USB flash drive, etc. A GostCrypt"
                                    +" container is just like any normal file (it can be, for example, moved or deleted as"
                                    +" any normal file). Click 'Select File' to choose a filename for the container and"
                                    +" to select the location where you wish the container to be created.<br><br><b>WARNING</b>: If you select"
                                    +" an existing file, GostCrypt will NOT encrypt it; the file will be deleted and replaced with"
                                    +" the newly created GostCrypt container. You will be able to encrypt existing files (later"
                                    +" on) by moving them to the GostCrypt container that you are about to create now.")

    Text {
        id:titre
        y: 10
        font.pointSize: 13
        font.family: "Helvetica"
        text: qsTr("Create a new file that will contain your volume:") + Translation.tr
        anchors.horizontalCenter: parent.horizontalCenter
        color: custompalette.text
        wrapMode: Text.WordWrap
    }

    UI.ButtonBordered {
        id: buttonOpen
        anchors.top: titre.bottom
        anchors.topMargin: 20
        height: combo.height
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Select File...")
        width: 200
        onClicked: fileDialog.open()
        color_: custompalette.green
    }

    UI.HelpButton {
        size: combo.height
        anchors.left: buttonOpen.right
        anchors.leftMargin: 10
        y: buttonOpen.y
        onClicked: {
            openErrorMessage("Information", message)
        }
    }


    UI.CustomComboBox {
        id: combo
        width: parent.width - 100
        height: 30
        anchors.top: buttonOpen.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        model: {
            if (volumeInfos.VOLUME_PATH !== "") {
                return [ volumeInfos.VOLUME_PATH ]
            }
            return []
        }
        onActivated: {
            volumeInfos.VOLUME_PATH = currentText
        }
    }

    UI.ButtonBordered {
        id: buttonMount
        anchors.top: combo.bottom
        anchors.topMargin: 10
        height: combo.height
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Select Mountpoint...")
        width: 200
        onClicked: folderDialog.open()
        color_: custompalette.green
    }

    UI.HelpButton {
        size: combo.height
        anchors.left: buttonMount.right
        anchors.leftMargin: 10
        y: buttonMount.y
        onClicked: {
            openErrorMessage("Information", message) // TODO change message
        }
    }

    UI.CustomComboBox {
        id: mountpointbox
        width: parent.width - 100
        height: 30
        anchors.top: buttonMount.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        model: {
            if (volumeInfos.VOLUME_MOUNTPOINT !== "") {
                return [ volumeInfos.VOLUME_MOUNTPOINT ]
            }
            return []
        }
        onActivated: {
            volumeInfos.VOLUME_MOUNTPOINT = currentText
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Please choose a file") + Translation.tr
        folder: shortcuts.home
        selectExisting: false
        onAccepted: {
            var path = fileDialog.fileUrl.toString();
            path = path.replace(/^(file:\/\/\/)/, "/");
            combo.model = [ path ]
            volumeInfos.VOLUME_PATH = path
            if (volumeInfos.VOLUME_MOUNTPOINT !== "" && volumeInfos.VOLUME_PATH !== "") {
                next.visible = true
            }
        }
        onRejected: {
        }
    }

    FileDialog {
        id: folderDialog
        title: qsTr("Please choose a folder") + Translation.tr
        folder: shortcuts.home
        selectFolder: true
        onAccepted: {
            var path = folderDialog.folder.toString();
            path = path.replace(/^(file:\/\/\/)/, "/");
            mountpointbox.model = [ path ]
            volumeInfos.VOLUME_MOUNTPOINT = path
            if (volumeInfos.VOLUME_MOUNTPOINT !== "" && volumeInfos.VOLUME_PATH !== "") {
                next.visible = true
            }
        }
        onRejected: {
        }
    }

    function setFileDialog(bool) {
        fileDialog.selectExisting = bool
    }

}
