import QtQuick 2.7
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4
import "../" as UI

Item {
    id: top
    property alias page : content.item
    property int currentPage: 1
    property var typeBranch
    property var volumeInfos: {
        "VOLUME_SIZE": [0, "MB"],                       //file size and type (KB, MB, GB) (standard volumes)
        "VOLUME_TYPE": 0,
        "VOLUME_PATH": "",                          //volume path (standard/hidden volume)
        "VOLUME_MOUNTPOINT": "",
        "VOLUME_PWD": "",                           //volume password (hidden+direct volume)
        "VOLUME_PWD_VERIF": "",
        "VOLUME_KEYFILES": [],                      //volume keyfile(s) (hidden+direct volume)
        "VOLUME_USE_KEYFILES": false, // TODO get parameter from UserSettings?
        "VOLUME_ALGORITHM": 0,                      //name of the algorithm
        "VOLUME_HASH": 0,                           // TODO clean struct
        "VOLUME_FS": 1,
    }
    property var progress: {
        "VOLUME_TYPE": 1,
        "VOLUME_PATH": 2,
        "VOLUME_ALGO": 3,
        "VOLUME_HASH": 4,
        "VOLUME_FS": 5,
        "VOLUME_SIZE": 6,
        "VOLUME_PWD": 7,
        "VOLUME_END": 666
    }

    //All choices possibilities
    //Format : "page id / loading bar value"
    property var pathBase: [{0: 1, 1: 1, 2: qsTr("Volume Type"), 3: [false, true]},
                            {0: 2, 1: 2, 2: qsTr("Volume File"), 3: [true, false]},
                            {0: 3, 1: 3, 2: qsTr("Encryption Algorithm"), 3: [true, true]},
                            {0: 4, 1: 4, 2: qsTr("Key Derivation Function"), 3: [true, true]},
                            {0: 5, 1: 5, 2: qsTr("FUSE File System"), 3: [true, true]},
                            {0: 6, 1: 6, 2: qsTr("Volume Size"), 3: [true, false]},
                            {0: 7, 1: 7, 2: qsTr("Password selection"), 3: [true, false]}]

    property var path: {"type": pathBase, "position": 0}

    signal next()
    signal back()
    signal cancel()
    signal help()

    x:0
    y:0
    anchors.topMargin: 0

    Loader {
        id:content
        source: "Page1.qml"
        x:0
        y:0
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width-40
        height: parent.height-40

        onSourceChanged: animation.running = true

                NumberAnimation {
                    id: animation
                    target: loader.item
                    property: "opacity"
                    from: 0
                    to: 100
                    duration: app.duration
                    easing.type: Easing.InExpo
                }
    }

    Row {
        id: steps
        spacing: 25
        anchors.horizontalCenter: parent.horizontalCenter
        y: 275
        StepComponent {
            id: step1
            checked: true
        }
        StepComponent {
            id: step2
        }
        StepComponent {
            id: step3
        }
        StepComponent {
            id: step4
        }
        StepComponent {
            id: step5
        }
        StepComponent {
            id: step6
        }
        StepComponent {
            id: step7
        }
        StepComponent {
            id: step8
        }
    }

    Row {
        spacing: 22
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: steps.top
        anchors.topMargin: -5
        anchors.right: top.right;
        anchors.rightMargin: 5

        UI.ButtonBordered {
            id: help_
            height: 25
            width: 80
            color_: custompalette.blue
            text: qsTr("Help")
            // TODO add help when pressed here
        }
    }

    NextPreviousButton {
        id: previous
        x: -60
        y: 270/2 - 70
        onPressed: manageWizard(-1)
        visible: false
    }

    NextPreviousButton {
        id: next
        x: parent.width + 20
        y: 270/2 - 70
        type: true
        onPressed: manageWizard(1)
        visible: true
    }

    function manageButtons(a_,b_)
    {
        previous.visible = a_;
        next.visible = b_;
    }

    function manageWizard(direction)
    {
        switch(currentPage)
        {

        /* Page7.qml
         * Volume password
         */
        case progress.VOLUME_PWD:
            if (direction == 1) {
                path.position += direction;
                content.source = "PageEnd.qml"
                manageButtons(true, false);
                manageProgressBar(8)
                break;
            }

        /* all pages */
        default:
            path.position += direction;
            changePage()
            break;
        }
    }

    /*!
      * Fn : manages the page loading
      */
    function changePage() {
        content.source = "Page"+path.type[path.position][0]+".qml"
        changeSubWindowTitle(path.type[path.position][2])
        manageButtons(path.type[path.position][3][0], path.type[path.position][3][1]);
        currentPage = path.type[path.position][0]
        manageProgressBar(path.type[path.position][1])

        switch(currentPage) {
        case progress.VOLUME_PATH:
            if (volumeInfos.VOLUME_PATH !== "") {
                next.visible = true
            }
            break;
        case progress.VOLUME_SIZE:
            sendGetAvailableSpace(volumeInfos.VOLUME_PATH.replace(/^(file:\/\/\/)/, "/"));
            if (volumeInfos.VOLUME_SIZE[0] > 0) {
                next.visible = true
            }
            break;
        case progress.VOLUME_PWD:
            if (volumeInfos.VOLUME_PWD !== "" && (volumeInfos.VOLUME_PWD === volumeInfos.VOLUME_PWD_VERIF) ) {
                next.visible = true
            }
            break;
        default:
            break;
        }

    }

    function createVolume()
    {
        var param, i;
        var size = volumeInfos.VOLUME_SIZE[0];

        switch (volumeInfos.VOLUME_SIZE[1]) {
            case "TB":
                size *= 1024;
            case "GB":
                size *= 1024;
            case "MB":
                size *= 1024;
            case "KB":
                size *= 1024;
            default:
        }

        param = {
            "type": app.volumetypes["id"][volumeInfos.VOLUME_TYPE],
            "path": volumeInfos.VOLUME_PATH,
            "mountpoint": volumeInfos.VOLUME_MOUNTPOINT,
            "size": size,
            "algorithm": app.algorithms["id"][volumeInfos.VOLUME_ALGORITHM],
            "hash": app.hashs["id"][volumeInfos.VOLUME_HASH],
            "filesystem": app.filesystems["id"][volumeInfos.VOLUME_FS],
            "nb-keyfiles": 0,
            "keyfiles": [],
            "password": volumeInfos.VOLUME_PWD,
            "name": qsTr("Create volume"),
            "desc": qsTr("Creating the ") + volumeInfos.VOLUME_PATH + qsTr(" volume...")
        }

        for(i in volumeInfos.VOLUME_NEW_KEYFILES)
            param["keyfile"+i] = volumeInfos.VOLUME_NEW_KEYFILES[i];

        qmlRequest("CreateVolume", param);
    }

    /*!
      * Fn : manages the loading bar
      */
    function manageProgressBar(value)
    {
        step1.checked = false
        step2.checked = false
        step3.checked = false
        step4.checked = false
        step5.checked = false
        step6.checked = false
        step7.checked = false
        step8.checked = false

        switch(value)
        {
        case 1:
            step1.checked = true
            break;
        case 2:
            step2.checked = true
            break;
        case 3:
            step3.checked = true
            break;
        case 4:
            step4.checked = true
            break;
        case 5:
            step5.checked = true
            break;
        case 6:
            step6.checked = true
            break;
        case 7:
            step7.checked = true
            break;
        case 8:
            step8.checked = true
            break;
        }
    }

    function updateAvailableSpace(size) {
        content.item.updateAvailableSpace(size)
    }

}
