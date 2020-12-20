import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Rectangle {
    property int height_
    id:titlebar
    width: parent.width
    color: custompalette.border
    anchors.bottomMargin: 1

    Rectangle {
        anchors.top: titlebar.bottom
        height: 1
        width: parent.width
        color: custompalette.border
    }

    Rectangle {
        id: logoTop
        x:10
        y:10
        height: 20
        Image {
            fillMode: Image.PreserveAspectFit
            source: "ressource/logo_gostcrypt.png"
            height:20
        }

        Text {
            id: text
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("GostCrypt 2.0") + Translation.tr
            color: custompalette.text
            x:20
            font.pixelSize: 13
        }

        Image {
            id: link
            anchors.verticalCenter: parent.verticalCenter
            x: text.x + text.width + 10
            fillMode: Image.PreserveAspectFit
            source: "ressource/link.png"
            width:22
            MouseArea {
                anchors.fill: link
                hoverEnabled: true;
                onClicked: Qt.openUrlExternally("http://www.gostcrypt.org/")
                cursorShape: Qt.PointingHandCursor
            }
        }
    }

    Rectangle{
        id:appclose
        height: height_
        width: height_
        anchors.right: parent.right
        color: custompalette.border
        Text{
            color: custompalette.text
            text: "×"
            anchors.centerIn: parent
            font.pointSize: 15
        }
        MouseArea{
            width: parent.width
            height: parent.height
            hoverEnabled: true
            onEntered: appclose.color=custompalette.red
            onExited: appclose.color=custompalette.border
            onClicked: {
                exitRequested = true
               // openErrorMessage("Information", "WARNING: Close GostCrypt dismounts all volumes currently mounted on the system. Continue anyway?")
              //  qmlRequest("dismountall", "");
                app.appQuit();
            }
        }
    }

    Rectangle{
        id:appminimize
        height: height_
        width: height_
        anchors.right: appclose.left
        color: custompalette.border
        Text{
            color: custompalette.text
            text: '_'
            anchors.horizontalCenter: parent.horizontalCenter
            font.pointSize: 15
        }
        MouseArea{
            width: parent.width
            height: parent.height
            hoverEnabled: true
            onEntered: appminimize.color=custompalette.darkThird
            onExited: appminimize.color=custompalette.border
            onClicked: app.visibility = Window.Minimized
        }
    }

    Button {
        property color backgroundColor: notifications.checked ? custompalette.green : notifications.hovered ? custompalette.green : "transparent"
        id: notifications
        y:0
        height: height_
        width: height_
        anchors.right: appminimize.left
        x: 20
        checkable: true
        checked: false
        onClicked: title.changeNotif(notifications.checked);
        style: ButtonStyle {
            id: notificationsStyle
                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    color: notifications.backgroundColor
                }
            }

        Image {
            id: notifications_image
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
            source: "ressource/notification.png"
            height: 20
            width: 20
        }

        Rectangle {
            id: unread_notifs
            visible: false
            width: 10
            height: 10
            color: custompalette.green
            radius: 10
            x:20
            y:25
            border.width: 1
            border.color: "#1a1a1a"
        }

        transitions: Transition {
            ColorAnimation { duration:app.duration/2 }
        }


    }

    function showIcon(bool)
    {
       unread_notifs.visible = bool;
    }

    function changeNotif(bool) {
        if(bool) {
            notifs.opacity = 1.0;
            bk_notifs.enabled = true;
            notifs.printNotification();
            notifs.drawNotification();
            showIcon(false);
        }
        else
        {
            notifications.checked = false;
            notifs.opacity = 0.0;
            bk_notifs.enabled = false;
        }       
    }
}
