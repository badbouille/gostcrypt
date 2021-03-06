import QtQuick 2.7
import QtQuick.Dialogs 1.2
import "../" as UI

Item {
    id: top;
    signal generateKeyfile()

    Connections {
        target: top
        onUpdate: {
            catchClose();
        }
    }

    Column {
        y: 5
        anchors.centerIn: parent
        width: 500
        spacing: 10
      Row {
          anchors.topMargin: 10
          anchors.horizontalCenter: parent.horizontalCenter
          spacing: 15
          Text {
              id: text2
              color: custompalette.text
              text: qsTr("Mixing PRF:") + Translation.tr
              font.pointSize: 11
          }
          UI.CustomComboBox {
              id: pRFType
              model: ["GOST R 34.11-2012","GOST R 31.11-94", "Whirlpool"]
              width: 200
              height: 30
          }
      }
      UI.ButtonBordered {
          id: saveButton
          height: 40
          anchors.horizontalCenter: parent.horizontalCenter
          text: qsTr("Generate and save keyfile...") + Translation.tr
          width: 230
          onClicked: fileDialog.open()
          color_: custompalette.green
          anchors.bottomMargin: 10
      }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Please choose a filepath") + Translation.tr
        folder: shortcuts.home
        selectExisting: false
        onAccepted: {
            var param ={}
            param["prf"] = pRFType.text
            param["keyfile"] = fileDialog.fileUrl
            param["name"] = qsTr("Keyfile Generator");
            param["desc"] = qsTr("Generating ") + fileDialog.fileUrl
            qmlRequest("CreateKeyFile", param);
            top.update();
        }
        onRejected: {
        }
    }
}
