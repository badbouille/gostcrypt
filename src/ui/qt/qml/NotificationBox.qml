import QtQuick 2.8
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2

Item {
    id: top
    width: 250
    height:292

    function printNotification() {
       if(notifications.length > 0)
          notif_title.text = notifications.length + " " + qsTr("NOTIFICATION(S)") + Translation.tr;
       else
          notif_title.text = qsTr("NO NOTIFICATION") + Translation.tr;
    }

    function drawNotification() {
        //Removing all the delegate items to clear the listOfNotifications
        //and run the animation when the user opens the Notification box
        listOfNotifications.clear();

        for(var notif in app.notifications)
        {
            listOfNotifications.insert(
                        0,
                        {
                            Notif_id: Number(app.notifications[notif][2]),
                            Notif_name: String(app.notifications[notif][0]),
                            Notif_percent: String(app.notifications[notif][3]),
                            Notif_desc: String(app.notifications[notif][1]),
                            Notif_displayed: 0
                        });
        }

    }

    function updateNotification(id,percent)
    {
        //Checking if the notification exists
        for(var i = 0; i < app.notifications.length; ++i)
            if(app.notifications[i][2] === id) {
                app.notifications[i][3] = percent;
                listOfNotifications.setProperty(app.notifications.length-i-1, "Notif_percent", percent)
                if(app.notifications[i][4] === 0) {
                    timerNotifPreview.start();
                    app.notifications[i][4] = 1;
                }
                return;
            }
    }

    MouseArea {
        id: no_hover
        anchors.fill: parent
        hoverEnabled: true
    }

    Rectangle {
        id: form
        y: 11
        color: custompalette.border
        width:parent.width
        height: parent.height
        Text {
            id: notif_title
            text: ""
            anchors.horizontalCenter: parent.horizontalCenter
            y: 0
            height: 30
            color: custompalette.text
            lineHeight: 30
            verticalAlignment: Text.AlignVCenter
        }
        border.width: 1
        border.color: "#1a1a1a"
    }

    Image {
        fillMode: Image.PreserveAspectFit
        source: "ressource/triangle_notif.png"
        height:12
        x: 177
        y: 0
    }

    Rectangle {
        id: form2
        y: 42
        color: custompalette.darkSecond
        width: 248
        x: 1
        height: parent.height -31

        Component {
            id: notification_delegate
            SwipeDelegate {
                id: delegate
                property int id: Notif_id
                width: 248; height: body.height
                clip: true
                background: Item {
                    Rectangle { //body
                        id: body
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.fill: parent
                        color: custompalette.darkSecond
                        height: 80
                        Text {
                            id: delegateText
                            text: (Number(Notif_percent) === -1) ? qsTr("<b>Double-click</b> to display the error that occurred.") : "<b>"+ Notif_name +"</b><br>" + Notif_desc
                            wrapMode: Text.WordWrap
                            width: parent.width - 50
                            leftPadding: 20
                            rightPadding: 20
                            clip: true
                            x: (Number(Notif_percent) !== -1) ? 60 : 0
                            anchors.verticalCenter: parent.verticalCenter
                            color: custompalette.text
                        }

                        CircleLoadingBar {
                            id: circle
                            anchors.verticalCenter: parent.verticalCenter
                            x: 10
                            size: 50
                            visible: (Number(Notif_percent) !== -1) ? true : false
                            percent: Number(Notif_percent)
                        }

                        MouseArea {
                            anchors.fill: parent
                            onDoubleClicked: {
                                openErrorMessage("Information", Notif_desc)
                                title.changeNotif(false)
                            }
                        }
                    }

                    Rectangle { //border
                        height:1
                        width: 248
                        anchors.bottom: parent.bottom
                        color: custompalette.border
                    }
                }

                enabled:  (Notif_percent == 100 || Number(Notif_percent) === -1) ? true : false
                swipe.right: removeComponent
                swipe.onCompleted: {
                    if(swipe.position === -1.0 && index !== -1) listOfNotifications.remove(index)
                    for(var i = 0; i<app.notifications.length; i++)
                        if (app.notifications[i][2] === delegate.id)
                            app.notifications.splice(i, 1)
                    printNotification();
                }

                Component {
                    id: removeComponent
                    Label {
                        id: deleteLabel
                        text: qsTr("Removing the notification...")
                        color: "white"
                        verticalAlignment: Label.AlignVCenter
                        padding: 12
                        height: parent.height
                        width: parent.width
                        anchors.right: parent.right

                        background: Rectangle {
                            color: custompalette.darkInput
                        }
                    }
                }

                swipe.transition:
                    Transition {
                        SmoothedAnimation {
                            velocity: 3;
                            easing.type:
                                Easing.InOutCubic
                        }
                    }

                ListView.onRemove: SequentialAnimation {
                    PropertyAction {
                        target: delegate
                        property: "ListView.delayRemove"
                        value: true
                    }
                    NumberAnimation {
                        target: delegate
                        property: "height"
                        to: 0
                        easing.type: Easing.InOutQuad
                        duration: {
                            //Prevent graphical issue
                            if(notifs.opacity !== 1.0) return 0
                            else return app.duration/2
                        }
                    }
                    PropertyAction {
                        target: delegate;
                        property: "ListView.delayRemove";
                        value: false
                    }
                }
            }
        }

        ListView {
            id: listview
            width: parent.width;
            height: parent.height - 7
            model: ListModel {
                id: listOfNotifications
                dynamicRoles: true
            }
            delegate: notification_delegate
            focus: true
            ScrollBar.vertical: ScrollBar {
                snapMode: ScrollBar.SnapOnRelease
                policy: ScrollBar.AlwaysOn
            }
            clip: true
        }
    }

    Rectangle {
        id: form3
        y: parent.height+4
        width: 248
        x:1
        color: custompalette.green
        height: 7
    }

    Behavior on opacity {
        NumberAnimation {
            id: anim_;
            duration: app.duration/2;
            easing.type: Easing.OutQuad;
            onRunningChanged: {
                top.visible = true
                if(!anim_.running && top.opacity === 0.0) {
                    top.visible = false
                }
            }
        }
    }

}
