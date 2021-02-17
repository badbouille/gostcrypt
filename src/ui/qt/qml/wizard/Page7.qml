import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.4
import "../" as UI

Item {
    id: top
    anchors.centerIn: parent
    property variant password: ["", ""]
    property int type: 0
    property bool isFile: volumeInfos.VOLUME_USE_KEYFILES
    property string text_: qsTr("     It is very important that you choose a good password. You should avoid"
                                 +" choosing one that contains only a single word that can be found in a dictionary (or a combination of 2, 3 or 4 such words)."
                                 +" It should not contain any names or dates of birth. It sould not be easy to guess."
                                 +" A good password is a random combination of upper and lower case letters, numbers, and special characters"
                                 +", such as @  = $ * + etc. We recommend choosing a password consisting of more than 20 characters (the longer, the better)."
                                 +" The maximum possible length is 64 characters.")

    FileDialog {
        id: addKeyfiles
        title: qsTr("Please choose a keyfile") + Translation.tr
        folder: shortcuts.home
        selectMultiple: true
        onAccepted: {
            var text = "";
            if(addKeyfiles.fileUrls.length > 0) use_Keyfiles.checked = true;
            for(var path in addKeyfiles.fileUrls) {
                var p = addKeyfiles.fileUrls[path].replace(/^(file:\/\/\/)/, "/");
                volumeInfos.VOLUME_KEYFILES.push(p);
            }
            for(var i in volumeInfos.VOLUME_KEYFILES) {
                text = text + volumeInfos.VOLUME_KEYFILES[i] + "; ";
            }
            keyfiles_paths.model = volumeInfos.VOLUME_KEYFILES;
        }
        onRejected: {
        }
    }

    Text {
        id:titre
        font.pointSize: 13
        font.family: "Helvetica"
        text: qsTr("Please enter a password twice or use Keyfiles") + Translation.tr
        anchors.horizontalCenter: parent.horizontalCenter
        color: custompalette.text
        wrapMode: Text.WordWrap
    }

    Row {
        width: parent.width - 50
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10
        anchors.top: titre.bottom
        anchors.topMargin: 20

        Item {
            id: modePassword
            width: (isFile) ? 260 : parent.width
            height: 150
            UI.SecureTextField {
                id: password_value
                width: (isFile) ? top.width*0.5 : top.width
                horizontalAlignment: TextInput.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
                echoMode: TextInput.Password
                height: 40
                focus: true
                text: volumeInfos.VOLUME_PWD
                onTextChanged: {
                    volumeInfos.VOLUME_PWD = password_value.text
                    if (password_value2.text !== volumeInfos.VOLUME_PWD) {
                        setpasswordstyle(false)
                    } else {
                        setpasswordstyle(true)
                    }
                }
                //Keys.onReleased: password[0] = password_value.text
                bordercolor: custompalette.darkInput
                radius_: 3
            }

            UI.SecureTextField {
                id: password_value2
                anchors.top: password_value.bottom
                anchors.topMargin: 10
                width: (isFile) ? top.width*0.5 : top.width
                horizontalAlignment: TextInput.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
                echoMode: TextInput.Password
                height: 40
                focus: true
                bordercolor: custompalette.darkInput
                radius_: 3
                text: volumeInfos.VOLUME_PWD_VERIF
                onTextChanged: {
                    volumeInfos.VOLUME_PWD_VERIF = password_value2.text
                    if (password_value2.text !== volumeInfos.VOLUME_PWD) {
                        setpasswordstyle(false)
                    } else {
                        setpasswordstyle(true)
                    }
                }
                //Keys.onReleased: password[1] = password_value2.text
                //Keys.onReturnPressed: manageWizard(1)
                //Keys.onEnterPressed: manageWizard(1)
            }
        }

        Item {
            id: modeFile
            width: 260
            height: 150
            visible: isFile
            UI.ButtonBordered {
                id: buttonKeyfiles
                anchors.horizontalCenter: parent.horizontalCenter
                height: 40
                text: qsTr("Open Keyfiles...")
                width: 150
                color_: custompalette.green
                onClicked: addKeyfiles.open()
            }
            UI.CustomComboBox {
                id: keyfiles_paths
                model: volumeInfos.VOLUME_KEYFILES
                anchors.top: buttonKeyfiles.bottom
                anchors.topMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter
                width: 250
                borderWidth: 0
                delegate:
                    ItemDelegate {
                        id: itemDelegate
                        width: keyfiles_paths.width
                        background: Rectangle {
                            color: itemDelegate.down ? custompalette.border : "transparent"
                            height: 10
                            width: keyfiles_paths.width
                            Text {
                                id: close
                                x: 10
                                color: custompalette.text
                                text: "×"
                                width: 30
                                anchors.verticalCenter: parent.verticalCenter
                                font.pointSize: 15
                            }
                            Text {
                                x: 30
                                width: parent.width - 50
                                text: modelData
                                elide: Text.ElideRight
                                anchors.verticalCenter: parent.verticalCenter
                                font.pointSize: 9
                                color: custompalette.green
                            }
                            MouseArea {
                                id: delegateItem
                                hoverEnabled: true
                                anchors.fill: parent
                            }
                            MouseArea {
                                id: closeItem
                                hoverEnabled: true
                                anchors.fill: close
                                onClicked: {
                                    if (index > -1) {
                                        volumeInfos.VOLUME_KEYFILES.splice(index, 1)
                                        keyfiles_paths.model = volumeInfos.VOLUME_KEYFILES
                                    }
                                }
                            }
                            ToolTip {
                                parent: parent
                                text: modelData
                                visible: delegateItem.containsMouse
                                delay: 500
                                timeout: 5000
                            }
                            ToolTip {
                                parent: parent
                                text: qsTr("Remove this keyfile from the list")
                                visible: closeItem.containsMouse
                                delay: 500
                                timeout: 5000
                            }
                        }
                        highlighted: keyfiles_paths.highlightedIndex === index
                }
            }

            Rectangle {
                anchors.left: keyfiles_paths.right
                anchors.leftMargin: 5
                color: custompalette.darkInput
                height: 20
                width: 20
                radius: 5
                border.width: 1
                anchors.verticalCenter: keyfiles_paths.verticalCenter
                border.color: custompalette.blue
                Text {
                    anchors.centerIn: parent;
                    text: qsTr("×")
                    font.pointSize: 15
                    color: custompalette.text
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        volumeInfos.VOLUME_KEYFILES = []
                        keyfiles_paths.model = []
                    }
                }
            }

        }
    }

    UI.CheckBox {
        id: display
        text_: qsTr("Display password")
        y: 150
        anchors.horizontalCenter: parent.horizontalCenter
        height: 20
        size_: 20
        checked: {
            var isChecked = UserSettings.getSetting("MountV-ShowPassword")
            return (isChecked === "1") ? true : false;
        }
        onCheckedChanged: {
            if(display.checked == true) {
                UserSettings.setSetting("MountV-ShowPassword", 1)
                password_value.echoMode = TextInput.Normal;
                password_value2.echoMode = TextInput.Normal;
            } else {
                UserSettings.setSetting("MountV-ShowPassword", 0)
                password_value.echoMode = TextInput.Password;
                password_value2.echoMode = TextInput.Password;
            }
        }

    }

    UI.CheckBox {
        id: use_Keyfiles
        text_: qsTr("Use Keyfiles...") + Translation.tr
        anchors.top: display.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        height: 20
        size_: 20
        checked: isFile
        onCheckedChanged: {
            if(use_Keyfiles.checked == true) {
                volumeInfos.VOLUME_USE_KEYFILES = true
                isFile = volumeInfos.VOLUME_USE_KEYFILES
                UserSettings.setSetting("MountV-UseKeyFiles", 1)
            } else {
                volumeInfos.VOLUME_USE_KEYFILES = false
                isFile = volumeInfos.VOLUME_USE_KEYFILES
                UserSettings.setSetting("MountV-UseKeyFiles", 0)
            }
        }
    }

    UI.HelpButton {
        id:description
        size: 30
        anchors.left: titre.right
        anchors.leftMargin: 10
        y: titre.y
        onClicked: {
            openErrorMessage("Information", text_)
        }
    }

    function setpasswordstyle(ok) {
        if(ok) {
            password_value2.textcolor = custompalette.text
            password_value2.bordercolor = custompalette.borderInput
            password_value2.backgroundcolor = custompalette.darkInput
            next.visible = (password_value2.text !== "" || password_value.text !== "") ? true : false;
        } else {
            password_value2.textcolor = "#e1e1e1"
            password_value2.bordercolor = "#e84747"
            password_value2.backgroundcolor = '#181818'
            next.visible = false;
        }
    }

}
