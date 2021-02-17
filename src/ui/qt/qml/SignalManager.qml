import QtQuick 2.0

Item {
    /*!
        \title Connections
        \brief QML slots that receive C++ signals
     */
    Connections {
        /*!
          Connects the C++ object (connectSignals.h) to the window
          */
        target: ConnectSignals;

        onConnectFinished: {
            //Getting the list of mounted volumes, algorithm(s), filesystem(s) and hash(s)
            qmlRequest("GetMountedVolumes", "");
            qmlRequest("GetEncryptionAlgorithms", "");
            qmlRequest("GetDerivationFunctions", "");
            qmlRequest("GetFilesystems", "");
            qmlRequest("GetVolumeTypes", "");
        }

        onSprintGetMountedVolumes: {
            subWindow.catchClose();
            pageLoader.item.clearVolumes();
            if(l.length === 0 && exitRequested === true && errorMessage.visible === false)
                app.appQuit();
            manageModel(l);
        }

        onSprintDismountVolume: {
            qmlRequest("GetMountedVolumes", "")
        }

        onSprintMountVolume: {
            qmlRequest("GetMountedVolumes", "")
        }

        onSprintCreateVolume: {
            qmlRequest("GetMountedVolumes", "")
        }

        onSprintSendError: {
            //openErrorMessage(l[0], l[1]);
            addNotification("error", l[0], l[1])
        }

        onSprintGetFileSystem: {
            app.filesystems.id = l.id
            app.filesystems.name = l.name
            app.filesystems.description = l.description
        }

        onSprintGetVolumeTypes: {
            app.volumetypes.id = l.id
            app.volumetypes.name = l.name
            app.volumetypes.description = l.description
        }

        onSprintGetDerivationFunctions: {
            app.hashs.id = l.id
            app.hashs.name = l.name
            app.hashs.description = l.description
        }

        onSprintGetEncryptionAlgorithms:
        {
            app.algorithms.id = l.id
            app.algorithms.name = l.name
            app.algorithms.description = l.description
        }

        onSprintGetHostDevices:
        {
            var i = 0
            for(var a in l)
            {
                subWindow.getLoader().item.addHostDevice(
                  {
                    number: i,
                    mountPoint: l[i]["mountPoint"],
                    path: l[i]["path"],
                    size: l[i]["size"]
                  })
                i++;
            }
        }

        onSprintProgressUpdate:
        {
            if(l === undefined) return;
            notifs.updateNotification(l["id"],l["progress"]*100);
            if(notifs.visible === false) title.showIcon(true);
            notifPreview.n = notifications[l["id"]-1][0]
            notifPreview.p = l["progress"]*100
        }

        onSprintBackupHeaderComplete:
        {
            openErrorMessage(qsTr("Backup complete !"), qsTr("Volume header backup has been successfully created;<br><br>IMPORTANT: "+
                                                             "Restoring the volume header using this backup will also restore the current "+
                                                             "volume password. Moreover, if keyfile(s)are/is necessary to mount the volume, "+
                                                             "the same keyfile(s) will be necessary to mount the volume again when the volume "+
                                                             "header is restored.<br><br>Warning: This volume header backup may be used to restore "+
                                                             "the header ONLY of this particular volume. If you use this header backup to resotre a "+
                                                             "header of a different volume, you will be able to mount the volume, but you will NOT be "+
                                                             "able to decrypt any data stored in the volume (because you will change its master key)."))
        }

        onSprintBenchmarkAlgorithms:
        {
            subWindow.getLoader().item.updateTableview(l);
        }

        onSprintGetAvailableSpace:
        {
            subWindow.getLoader().item.updateAvailableSpace(l)
        }

    }

    function manageModel(volumes)
    {
        app.model = volumes;
        //Delete all the model entries that are not in the volumes array
        for(var k in volumes)
        {
            pageLoader.item.loadVolume(
                        volumes[k]["mountPoint"],
                        volumes[k]["algo"],
                        volumes[k]["volumePath"],
                        volumes[k]["volumeSize"]);
        }
    }
}
