#include "GraphicInterface.h"

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

    QQmlContext* ctx = mEngine.rootContext();

    //ctx->setContextProperty("ConnectSignals", this);
    ctx->setContextProperty("UserSettings", &mSettings);
    ctx->setContextProperty("DragWindowProvider", &mDrag);
    ctx->setContextProperty("Translation", (QObject*)&mTranslation);
    qmlRegisterType<SecureTextField>("gostcrypt.ui.secureInput", 1, 0, "SecureTextField");

    mEngine.load(QUrl(QStringLiteral("qrc:/UI/main.qml")));

    return mApp->exec();
}

QString GraphicInterface::formatSize(quint64 sizeInByte, bool withFontColor)
{
    return UserInterface::formatSize(sizeInByte, withFontColor);
}

