/**
 * @file GraphicInterface.cpp
 * @author seragonia
 * @date 20/01/2019
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <Core.h>
#include "GraphicInterface.h"
#include "commonDefines.h"
#include "QThreads.h"

GraphicInterface *current_instance;

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

    mEngine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

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

    GostCrypt::Core::CreateParams_t *arguments = new GostCrypt::Core::CreateParams_t;
    GostCrypt::SecureBuffer *pass = new GostCrypt::SecureBuffer(64);
    uint32_t id;

    id = GI_KEY(aContent, "id").toUInt();

    /* Default init */
    arguments->afterCreationMount.password = GostCrypt::SecureBufferPtr(pass->get(), pass->size());
    arguments->afterCreationMount.volumePath = "";
    arguments->afterCreationMount.mountPoint = "";
    arguments->afterCreationMount.fileSystemID = DEFAULT_FILESYSTEMID;

    arguments->password = GostCrypt::SecureBufferPtr(pass->get(), pass->size());
    arguments->volumePath = "";

    arguments->dataSize = DEFAULT_VOLUMESIZE;
    arguments->sectorSize = DEFAULT_SECTORSIZE;

    arguments->volumeTypeID = DEFAULT_VOLUMETYPE;
    arguments->algorithmID = DEFAULT_ALGORITHM;
    arguments->keyDerivationFunctionID = DEFAULT_KDF;

    /****************** PARSING PARAMETERS ********************/

    // type
    arguments->volumeTypeID = GI_KEY(aContent, "type").toString().toStdString();

    // path
    arguments->volumePath = GI_KEY(aContent, "path").toString().toStdString();
    arguments->afterCreationMount.volumePath = arguments->volumePath;

    // mountpoint
    arguments->afterCreationMount.mountPoint = GI_KEY(aContent, "mountpoint").toString().toStdString();

    // size
    arguments->dataSize = GI_KEY(aContent, "size").toULongLong();

    // algos
    arguments->algorithmID = GI_KEY(aContent, "algorithm").toString().toStdString();
    arguments->keyDerivationFunctionID = GI_KEY(aContent, "hash").toString().toStdString();
    arguments->afterCreationMount.fileSystemID = GI_KEY(aContent, "filesystem").toString().toStdString();

    // password
    QByteArray *a = new QByteArray(GI_KEY(aContent,"password").toString().toUtf8()); //Setting the outer volume password
    GostCrypt::SecureBufferPtr passptr((const uint8_t *)a->data(), a->length());
    arguments->password.copyFrom(passptr);
    //arguments->afterCreationMount.password.copyFrom(passptr);

    /************** REQUEST *****************/

    GostCrypt::Core::progress.setCallBack(uicallback, reinterpret_cast<void *>(id));

    QThread *t = new GostCrypt::CreateThread(arguments, pass);
    t->start();

}

void GraphicInterface::sendMountVolume(QVariant aContent)
{
#ifdef QT_DEBUG
    qDebug() << "Calling MountVolume";
#endif

    GostCrypt::Core::MountParams_t *arguments = new GostCrypt::Core::MountParams_t;
    GostCrypt::SecureBuffer *pass = new GostCrypt::SecureBuffer(64);
    uint32_t id;

    id = GI_KEY(aContent, "id").toUInt();

    arguments->mountPoint = "";
    arguments->password = GostCrypt::SecureBufferPtr(pass->get(), pass->size());
    arguments->fileSystemID = DEFAULT_FILESYSTEMID;
    arguments->volumePath = "";

    QString canonicalPath = GI_KEY(aContent, "path").toUrl().path();
    arguments->volumePath = canonicalPath.toStdString();

    QString mountpointPath = GI_KEY(aContent, "mountpoint").toUrl().path();
    arguments->mountPoint = mountpointPath.toStdString();

    QByteArray *a = new QByteArray(GI_KEY(aContent,"password").toString().toUtf8()); //Setting the outer volume password
    GostCrypt::SecureBufferPtr passptr((uint8_t *)a->data(), a->length());
    arguments->password.copyFrom(passptr);
    passptr.erase();

    GostCrypt::Core::progress.setCallBack(uicallback, reinterpret_cast<void *>(id));

    QThread *t = new GostCrypt::MountThread(arguments, pass);
    t->start();
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

    // must wait a little for the interface to process the changes
    // TODO this is unsatisfying
    //      maybe the umount command should wait for the volume to be completely unmounted
    QTime dieTime = QTime::currentTime().addMSecs(10);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    //

    emit QML_SIGNAL(printDismountVolume, QVariantList());

}

void GraphicInterface::sendDismountAllVolumes(QVariant aContent)
{
#ifdef QT_DEBUG
    qDebug() << "Calling DismountAll";
#endif
    (void)aContent;
    try {
        GostCrypt::Core::umountAll();
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
    QVariantMap r;
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

    l.clear();
    for (auto dea : deal) {
        l << QString::fromStdString(dea->GetName());
    }
    r.insert("name", l);

    l.clear();
    for (auto dea : deal) {
        l << QString::fromStdString(dea->GetDescription());
    }
    r.insert("description", l);

    l.clear();
    for (auto dea : deal) {
        l << QString::fromStdString(dea->GetID());
        delete dea;
    }
    r.insert("id", l);

    emit QML_SIGNAL(printGetEncryptionAlgorithms, r)
}

void GraphicInterface::sendGetDerivationFunctions(QVariant aContent)
{
#ifdef QT_DEBUG
    qDebug() << "GetDerivationFunctions";
#endif
    (void)aContent;
    QVariantMap r;
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

    l.clear();
    for (auto kdf : kdfl) {
        l << QString::fromStdString(kdf->GetID());
    }
    r.insert("id", l);

    l.clear();
    for (auto kdf : kdfl) {
        l << QString::fromStdString(kdf->GetName());
    }
    r.insert("name", l);

    l.clear();
    for (auto kdf : kdfl) {
        l << QString::fromStdString(kdf->GetDescription());
        delete kdf;
    }
    r.insert("description", l);

    emit QML_SIGNAL(printGetDerivationFunctions, r)
}

void GraphicInterface::sendGetFilesystems(QVariant aContent)
{
#ifdef QT_DEBUG
    qDebug() << "GetFilesystems";
#endif
    (void)aContent;
    QVariantMap r;
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

    filesystem.clear();
    for (auto f : fuseFileSystems) {
        filesystem << QString::fromStdString(f->getID());
    }
    r.insert("id", filesystem);

    filesystem.clear();
    for (auto f : fuseFileSystems) {
        filesystem << QString::fromStdString(f->getName());
    }
    r.insert("name", filesystem);

    filesystem.clear();
    for (auto f : fuseFileSystems) {
        filesystem << QString::fromStdString(f->getDescription());
        delete f;
    }
    r.insert("description", filesystem);

    emit QML_SIGNAL(printGetFileSystem, r)
}

void GraphicInterface::sendGetVolumeTypes(QVariant aContent)
{
#ifdef QT_DEBUG
    qDebug() << "GetVolumeTypes";
#endif
    (void)aContent;
    QVariantMap r;
    QVariantList l;
    GostCrypt::VolumeList volumes;

    try {
        volumes = GostCrypt::Core::GetVolumeTypes();
    } catch (GostCrypt::GostCryptException &e) {
        QVariantList r;
        r << e.name();
        r << e.what();
        emit QML_SIGNAL(printSendError, r)
        return;
    }

    l.clear();
    for (auto v : volumes) {
        l << QString::fromStdString(v->GetID());
    }
    r.insert("id", l);

    l.clear();
    for (auto v : volumes) {
        l << QString::fromStdString(v->GetName());
    }
    r.insert("name", l);

    l.clear();
    for (auto v : volumes) {
        l << QString::fromStdString(v->GetDescription());
        delete v;
    }
    r.insert("description", l);

    emit QML_SIGNAL(printGetVolumeTypes, r)
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

void GraphicInterface::sendGetAvailableSpace(QVariant p)
{
#ifdef QT_DEBUG
    qDebug() << "GetAvailableSpace";
#endif
    QString path = p.toString();
    QFileInfo file(path);
    path = file.dir().absolutePath();

    struct statvfs64 stats;

    memset(&stats, 0, sizeof(struct statvfs64));

    statvfs64(path.toStdString().c_str(), &stats);

    QString prefix = "B";
    uint64_t free_space = stats.f_bavail * stats.f_bsize;

    if (free_space > 1024) {
        prefix = "KB";
        free_space >>= 10;
    }

    if (free_space > 1024) {
        prefix = "MB";
        free_space >>= 10;
    }

    if (free_space > 1024) {
        prefix = "GB";
        free_space >>= 10;
    }

    if (free_space > 1024) {
        prefix = "TB";
        free_space >>= 10;
    }

    prefix = QString::number(free_space) + prefix;

    emit QML_SIGNAL(printGetAvailableSpace, prefix);
}

void GraphicInterface::printProgressUpdate(uint32_t requestId, qreal progress)
{
    QVariantMap progressData;
    progressData.insert("id", requestId);
    progressData.insert("progress", progress);
    emit QML_SIGNAL(printProgressUpdate, progressData)
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
    CONNECT_QML_SIGNAL(DismountAllVolumes);
    CONNECT_QML_SIGNAL(GetMountedVolumes);
    CONNECT_QML_SIGNAL(GetEncryptionAlgorithms);
    CONNECT_QML_SIGNAL(GetDerivationFunctions);
    CONNECT_QML_SIGNAL(GetFilesystems);
    CONNECT_QML_SIGNAL(GetVolumeTypes);
    CONNECT_QML_SIGNAL(GetHostDevices);
    CONNECT_QML_SIGNAL(CreateKeyFile);
    CONNECT_QML_SIGNAL(ChangeVolumePassword);
    CONNECT_QML_SIGNAL(BenchmarkAlgorithms);

    CONNECT_QML_SIGNAL(GetAvailableSpace);

    connect(qml, SIGNAL(sendAction(QString, QVariant)), this, SLOT(sendAction(QString, QVariant)));

    /* Saving current instance to emit signals from the callback */
    current_instance = this;

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

void GraphicInterface::uicallback(void *ctx, const char *msg, float progress)
{
    (void)msg;
    QVariantMap progressData;
    void *id[1];
    id[0] = ctx; // workaround to not have to instanciate context and just extact a uint32_t.
    progressData.insert("id", *((uint32_t*)id));
    progressData.insert("progress", progress);
    emit current_instance->sprintProgressUpdate(progressData);
}
