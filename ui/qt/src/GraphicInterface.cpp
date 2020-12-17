#include <Core.h>
#include "GraphicInterface.h"
#include "commonDefines.h"

GraphicInterface::GraphicInterface(MyGuiApplication* aApp, QObject* parent)
    : UserInterface(parent)
{
    mApp = aApp;
    mApp->setGI(this);
}

int GraphicInterface::start(int argc, char** argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    mApp->setWindowIcon(QIcon(":/logo_gostcrypt.png"));
    mApp->setApplicationName("GostCrypt");
    mApp->setOrganizationName("The GostCrypt Team"); // TODO use constants
    mApp->setOrganizationDomain("gostcrypt.org");

    QQmlContext* ctx = mEngine.rootContext();

    ctx->setContextProperty("ConnectSignals", this);
    ctx->setContextProperty("UserSettings", &mSettings);
    ctx->setContextProperty("DragWindowProvider", &mDrag);
    ctx->setContextProperty("Translation", (QObject*)&mTranslation);
    qmlRegisterType<SecureTextField>("gostcrypt.ui.secureInput", 1, 0, "SecureTextField");

    mEngine.load(QUrl(QStringLiteral("qrc:/UI/main.qml")));

    /* Connecting all signals */
    QMetaObject::invokeMethod((QObject *)this, "connectSignals", Qt::QueuedConnection);

    return mApp->exec();
}

QString GraphicInterface::formatSize(quint64 sizeInByte, bool withFontColor)
{
    return UserInterface::formatSize(sizeInByte, withFontColor);
}

void GraphicInterface::sendCreateVolume(QVariant aContent) {
#ifdef QT_DEBUG
    qDebug() << "Calling CreateVolume";
#endif

    GostCrypt::Core::CreateParams_t arguments;
    GostCrypt::SecureBuffer pass(64);

    /* Default init */
    arguments.afterCreationMount.password = GostCrypt::SecureBufferPtr(pass.get(), pass.size());
    arguments.afterCreationMount.volumePath = "";
    arguments.afterCreationMount.mountPoint = "";
    arguments.afterCreationMount.fileSystemID = DEFAULT_FILESYSTEMID;

    arguments.password = GostCrypt::SecureBufferPtr(pass.get(), pass.size());
    arguments.volumePath = "";

    arguments.dataSize = DEFAULT_VOLUMESIZE;
    arguments.sectorSize = DEFAULT_SECTORSIZE;

    arguments.volumeTypeID = DEFAULT_VOLUMETYPE;
    arguments.algorithmID = DEFAULT_ALGORITHM;
    arguments.keyDerivationFunctionID = DEFAULT_KDF;

    //Detection of the volume type
    int type = GI_KEY(aContent, "type").toInt(); //UI returns 0 for normal and 1 for Hidden
    if (type == 0)
    {
        arguments.volumePath = GI_KEY(aContent, "path").toString().toStdString();
        arguments.afterCreationMount.volumePath = arguments.volumePath;
        arguments.volumeTypeID = "standard";
        QByteArray *a = new QByteArray(GI_KEY(aContent,"password").toString().toUtf8()); //Setting the outer volume password
        GostCrypt::SecureBufferPtr passptr((const uint8_t *)a->data(), a->length());
        arguments.password.copyFrom(passptr);
        arguments.afterCreationMount.password.copyFrom(passptr);

        if (GI_KEY(aContent, "hash").toString() != "")
        {
            arguments.keyDerivationFunctionID = GI_KEY(aContent, "hash").toString().toStdString();    //Outer volume hash
        }

        if (GI_KEY(aContent, "algorithm").toString() != "")
        {
            arguments.algorithmID = GI_KEY(aContent,"algorithm").toString().toStdString();    //Outer volume algorithm
        }

        bool ok = false;

        QString s = GI_KEY(aContent, "size").toString();
        uint64_t v = UserInterface::parseSize(s, &ok); //Total volume file size
        arguments.dataSize = v;

        // TODO : add mountpoint and make the call

    }
}

void GraphicInterface::sendMountVolume(QVariant aContent)
{
#ifdef QT_DEBUG
    qDebug() << "Calling MountVolume";
#endif

    GostCrypt::Core::MountParams_t arguments;
    GostCrypt::SecureBuffer pass(64);

    arguments.mountPoint = "";
    arguments.password = GostCrypt::SecureBufferPtr(pass.get(), pass.size());
    arguments.fileSystemID = DEFAULT_FILESYSTEMID;
    arguments.volumePath = "";

    QString canonicalPath = GI_KEY(aContent, "path").toUrl().path();
    arguments.volumePath = canonicalPath.toStdString();

    QString mountpointPath = GI_KEY(aContent, "mountpoint").toUrl().path();
    arguments.mountPoint = mountpointPath.toStdString();

    QByteArray *a = new QByteArray(GI_KEY(aContent,"password").toString().toUtf8()); //Setting the outer volume password
    GostCrypt::SecureBufferPtr passptr((const uint8_t *)a->data(), a->length());
    arguments.password.copyFrom(passptr);

    try
    {
        GostCrypt::Core::mount(&arguments);
    }catch (GostCrypt::VolumePasswordException& e)
    {
        emit volumePasswordIncorrect();
        return;
    }
    catch (GostCrypt::GostCryptException &e) {
        QVariantList r;
        r << e.name();
        r << e.what();
        emit QML_SIGNAL(printSendError, r)
        return;
    }

    emit QML_SIGNAL(printMountVolume, QVariantList())
}

void GraphicInterface::sendGetMountedVolumes(QVariant aContent)
{
#ifdef QT_DEBUG
    qDebug() << "Calling GetMountedVolumes";
#endif
    (void)aContent;
    GostCrypt::Core::VolumeInfoList vlist;
    /* making the call */
    try {
        vlist = GostCrypt::Core::list();
    } catch (GostCrypt::GostCryptException &e) {
        QVariantList r;
        r << e.name();
        r << e.what();
        emit QML_SIGNAL(printSendError, r)
        return;
    }

    /* Sending results to interface */
    QVariantList list;
    for (auto & v : vlist)
    {
        QVariantMap vol;
        vol.insert("mountPoint", QString::fromStdString(v.mountPoint));
        vol.insert("algo", QString::fromStdString(v.algorithmID));
        vol.insert("volumePath", QString::fromStdString(v.file));
        vol.insert("volumeSize", formatSize(v.dataSize));
        list.append(vol);
    }
    emit QML_SIGNAL(printGetMountedVolumes, list)
}

void GraphicInterface::sendDismountVolume(QVariant aContent)
{
#ifdef QT_DEBUG
    qDebug() << "Calling Dismount";
#endif
    QString mountpoint;

    mountpoint = (GI_KEY(aContent, "path").toString());
    try {
        GostCrypt::Core::umount(mountpoint.toStdString());
    } catch (GostCrypt::GostCryptException &e) {
        QVariantList r;
        r << e.name();
        r << e.what();
        emit QML_SIGNAL(printSendError, r)
        return;
    }
    emit QML_SIGNAL(printDismountVolume, QVariantList());

}

void GraphicInterface::sendBenchmarkAlgorithms(QVariant aContent)
{
#ifdef QT_DEBUG
    qDebug() << "BenchmarkAlgorithms";
#endif
    (void)aContent;
    QVariantList r;
    r << "BenchmarkAlgorithms not implemented" << "Wait for the next update!";
    emit QML_SIGNAL(printSendError, r)
}

void GraphicInterface::sendGetEncryptionAlgorithms(QVariant aContent)
{
#ifdef QT_DEBUG
    qDebug() << "GetEncryptionAlgorithms";
#endif
    (void)aContent;
    QVariantList l;
    GostCrypt::DiskEncryptionAlgorithmList deal;

    try {
        deal = GostCrypt::Core::GetEncryptionAlgorithms();
    } catch (GostCrypt::GostCryptException &e) {
        QVariantList r;
        r << e.name();
        r << e.what();
        emit QML_SIGNAL(printSendError, r)
        return;
    }

    for (auto dea : deal) { // TODO send the fucking descriptions too
        l << QString::fromStdString(dea->GetID());
        delete dea;
    }

    emit QML_SIGNAL(printGetEncryptionAlgorithms, l)
}

void GraphicInterface::sendGetDerivationFunctions(QVariant aContent)
{
#ifdef QT_DEBUG
    qDebug() << "GetDerivationFunctions";
#endif
    (void)aContent;
    QVariantList l;
    GostCrypt::KDFList kdfl;

    try {
        kdfl = GostCrypt::Core::GetDerivationFunctions();
    } catch (GostCrypt::GostCryptException &e) {
        QVariantList r;
        r << e.name();
        r << e.what();
        emit QML_SIGNAL(printSendError, r)
        return;
    }

    for (auto kdf : kdfl) { // TODO send the fucking descriptions too
        l << QString::fromStdString(kdf->GetID());
        delete kdf;
    }

    emit QML_SIGNAL(printGetDerivationFunctions, l)
}

void GraphicInterface::sendGetFilesystems(QVariant aContent)
{
#ifdef QT_DEBUG
    qDebug() << "GetFilesystems";
#endif
    (void)aContent;
    QVariantList filesystem;
    GostCrypt::FuseFileSystemList fuseFileSystems;

    try {
        fuseFileSystems = GostCrypt::Core::GetFileSystems();
    } catch (GostCrypt::GostCryptException &e) {
        QVariantList r;
        r << e.name();
        r << e.what();
        emit QML_SIGNAL(printSendError, r)
        return;
    }

    for (auto f : fuseFileSystems) { // TODO send the fucking descriptions too
        filesystem << QString::fromStdString(f->getID());
        delete f;
    }

    emit QML_SIGNAL(printGetFileSystem, filesystem)
}

void GraphicInterface::sendGetHostDevices(QVariant aContent)
{
#ifdef QT_DEBUG
    qDebug() << "GetHostDevices";
#endif
    (void)aContent;
    QVariantList r;
    r << "GetHostDevices not implemented" << "Wait for the next update!";
    emit QML_SIGNAL(printSendError, r)
}

void GraphicInterface::sendCreateKeyFile(QVariant aContent)
{
#ifdef QT_DEBUG
    qDebug() << "CreateKeyFile";
#endif
    (void)aContent;
    QVariantList r;
    r << "CreateKeyFile not implemented" << "Wait for the next update!";
    emit QML_SIGNAL(printSendError, r)
}

void GraphicInterface::sendChangeVolumePassword(QVariant aContent)
{
#ifdef QT_DEBUG
    qDebug() << "ChangeVolumePassword";
#endif
    (void)aContent;
    QVariantList r;
    r << "ChangeVolumePassword not implemented" << "Wait for the next update!";
    emit QML_SIGNAL(printSendError, r)
}

void GraphicInterface::sendAction(QString name, QVariant aContent)
{
#ifdef QT_DEBUG
    qDebug() << "Action " + name;
#endif
    if (name == "download") {
        qDebug() << "Download button pressed";
        return;
    }
    if (name == "openmountpoint") {
        QDesktopServices::openUrl(QUrl(GI_KEY(aContent, "path").toString()));
        return;
    }
#ifdef QT_DEBUG
    qDebug() << "Unknown Action " + name;
#endif
}

void GraphicInterface::printProgressUpdate(quint32 requestId, qreal progress)
{
    QVariantList list;
    QVariantMap progressData;
    progressData.insert("id", requestId);
    progressData.insert("progress", progress);
    list.append(progressData);
    emit QML_SIGNAL(printProgressUpdate, list)
}

void GraphicInterface::connectSignals()
{
    /************* QML -----> C++ ************
     * Connects every signals from QML to the
     * "receiveSignal" slot of
     * the current class
    ******************************************/
    QObject* qml = mEngine.rootObjects().first();

    /***** GraphicInterface -----> Core ******/

    /* Connecting few exit signals to close the program apropriately */
    mApp->connect(this, SIGNAL(exited()), mApp, SLOT(quit()));
    connect(qml, SIGNAL(appQuit()), this, SLOT(exit()));
    mApp->connect(mApp, SIGNAL(askExit()), this, SLOT(exit()));

    /***** QML -----> GraphicInterface ******/
    CONNECT_QML_SIGNAL(CreateVolume);
    CONNECT_QML_SIGNAL(MountVolume);
    CONNECT_QML_SIGNAL(DismountVolume);
    CONNECT_QML_SIGNAL(GetMountedVolumes);
    CONNECT_QML_SIGNAL(GetEncryptionAlgorithms);
    CONNECT_QML_SIGNAL(GetDerivationFunctions);
    CONNECT_QML_SIGNAL(GetFilesystems);
    CONNECT_QML_SIGNAL(GetHostDevices);
    CONNECT_QML_SIGNAL(CreateKeyFile);
    CONNECT_QML_SIGNAL(ChangeVolumePassword);
    CONNECT_QML_SIGNAL(BenchmarkAlgorithms);

    connect(qml, SIGNAL(sendAction(QString, QVariant)), this, SLOT(sendAction(QString, QVariant)));

    //Notifying the QML that the signals are binded
    emit connectFinished();
}

bool MyGuiApplication::notify(QObject* receiver, QEvent* event)
{
    QVariantList response;
    bool done = true;

    try
    {
        done = QGuiApplication::notify(receiver, event);
    }
    catch (GostCrypt::GostCryptException& e)
    {
        response << e.name() << "An unexpected error occured. \n"
                                   + QString(e.what());
        emit mGI->QML_SIGNAL(printSendError, response)
    }
    catch (QException& e)     // TODO : handle exceptions here
    {
        response << "Exception catch" << "An unexpected error occured. \n"
                 << QString::fromUtf8(e.what());
        emit mGI->QML_SIGNAL(printSendError, response)
    }
    return done;
}

void GraphicInterface::exit()
{
    emit exited();
}
