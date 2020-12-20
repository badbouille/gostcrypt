#ifndef GRAPHIC_INTERFACE
#define GRAPHIC_INTERFACE

#include <QObject>
#include <QVariant>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "UserInterface.h"
#include "DragWindowProvider.h"
#include "UserSettings.h"
#include "TranslationApp.h"
#include "SecureTextField.h"
#include <QUrl>
#include <QString>

#define GI_KEY(variant, key) variant.toMap().value(key)
#define DEC_QML_PRINT_SIGNAL(requestName) void sprint ## requestName (QVariantList l);
#define CONNECT_QML_SIGNAL(requestName) connect(qml, SIGNAL(send ## requestName (QVariant)), this, SLOT(send ## requestName (QVariant)));
#define QML_SIGNAL(requestName, params) s ## requestName (params);

extern const char *g_prog_path;

class GraphicInterface;

/**
 * @brief
 * This class inherits QApplication and allows you to
 * retrieve events such as exceptions in order to manage
 * them graphically
 */
class MyGuiApplication : public QGuiApplication
{
    Q_OBJECT
 public:
    /**
     * @brief Initializes the QML application and class variables
     *
     * @param argc the number of arguments in the main program
     * @param argv arguments of the main program
     */
    MyGuiApplication(int& argc, char** argv) : QGuiApplication(argc, argv), mGI(nullptr) {}
    /**
     * @brief Retrieve exceptions and forward them to the interface for display
     *
     * @param receiver Object that receives the event
     * @param event Event that has occurred and must be transmitted
     * @return bool Boolean indicating whether the event has been transmitted or not
     */
    bool notify(QObject* receiver, QEvent* event) override;
    /**
     * @brief Retrieve a pointer on the graphical interface object
     *
     * @param gi Pointer to the currently instantiated QML interface
     */
    void setGI(GraphicInterface* gi) { mGI = gi; }
 signals:
    /**
     * @brief This signal makes it possible to finish the program neatly
     * by allowing the interface to tell the core that it will end
     */
    void askExit();
 private:
    GraphicInterface*
    mGI; /**< Pointer that will contain the address of the instantiated QML interface */
};

/**
 * @brief
 * This class displays a graphical interface using
 * QML for GostCrypt
 */
class GraphicInterface : public UserInterface
{
    Q_OBJECT
 public:
    /**
     * @brief Graphical user interface constructor, based on the MyGuiApplication class that inherits from QApplication
     *
     * @param aApp Parent object of the GUI: allows you to retrieve signals and start a QApplication
     * @param parent The object from which the class derives, which has useful methods for making certain calculations
     */
    explicit GraphicInterface(MyGuiApplication* aApp, QObject* parent = nullptr);
    /**
     * @brief Creates the graphical interface and sends all the necessary objects to it in context
     *
     * @param argc The number of arguments in the main program
     * @param argv Arguments of the main program
     * @return int Returns what the "exec" method returns at the end of the interface execution
     */
    int start(int argc, char** argv);
    /**
     * @brief Converts A size in bytes into a formatted character string (or not formatted)
     *
     * @param sizeInByte Size in bytes to convert
     * @param withFontColor This boolean allows you to specify whether you want an output formatted in HTML (true) or not (false)
     * @return QString Character string of the size converted to the desired format
     */
    static QString formatSize(quint64 sizeInByte, bool withFontColor = true);

 private slots:  // NOLINT

    /* SIGNAUX QML -------> QML_CORE */

    /* Signal pour quitter le programme */
    void exit();

    DEC_SEND_SLOT(CreateVolume);
    DEC_SEND_SLOT(MountVolume);
    DEC_SEND_SLOT(DismountVolume);
    DEC_SEND_SLOT(GetMountedVolumes);
    DEC_SEND_SLOT(GetEncryptionAlgorithms);
    DEC_SEND_SLOT(GetDerivationFunctions);
    DEC_SEND_SLOT(GetFilesystems);
    DEC_SEND_SLOT(GetHostDevices);
    DEC_SEND_SLOT(CreateKeyFile);
    DEC_SEND_SLOT(ChangeVolumePassword);
    DEC_SEND_SLOT(BenchmarkAlgorithms);

    /* Additional slots */
    virtual void sendAction(QString, QVariant);

    /**
     * @brief Display the new progress for the corresponding request in the UI
     */
    virtual void printProgressUpdate(quint32 requestId, qreal progress);

 signals:
    /**
     * @brief
     * Control signal sent to the interface to indicate that the signals are well connected
     */
    void connectFinished();

    /* SIGNAUX QML_CORE -------> QML */
    DEC_QML_PRINT_SIGNAL(CreateVolume)
    DEC_QML_PRINT_SIGNAL(MountVolume)
    DEC_QML_PRINT_SIGNAL(DismountVolume)
    DEC_QML_PRINT_SIGNAL(GetMountedVolumes)
    DEC_QML_PRINT_SIGNAL(GetEncryptionAlgorithms)
    DEC_QML_PRINT_SIGNAL(GetDerivationFunctions)
    DEC_QML_PRINT_SIGNAL(GetHostDevices)
    DEC_QML_PRINT_SIGNAL(GetFileSystem)
    DEC_QML_PRINT_SIGNAL(CreateKeyFile)
    DEC_QML_PRINT_SIGNAL(ChangeVolumePassword)
    DEC_QML_PRINT_SIGNAL(ProgressUpdate)
    DEC_QML_PRINT_SIGNAL(SendError)
    DEC_QML_PRINT_SIGNAL(BenchmarkAlgorithms)
    DEC_QML_PRINT_SIGNAL(BackupHeaderComplete)

    /**
     * @brief
     * Signal to indicate to the interface that the entered password is incorrect (volume)
     */
    void volumePasswordIncorrect();

    /**
     * @brief signal to tell Qt to exit the program
     */
    void exited();

 private:
    /**
     * @brief
     * Method callable from QML to connect signals between the interface and the Core
     */
    Q_INVOKABLE void connectSignals();
    MyGuiApplication* mApp; /**< Object inheriting the QApplication, the interface works thanks to this object */
    QQmlApplicationEngine mEngine; /**< QML rendering engine used to render the display, contains the main context */
    UserSettings mSettings; /**< User preferences */
    DragWindowProvider mDrag; /**< Class that allows you to move the window (which is borderless) */
    TranslationApp mTranslation; /**< Class to translate the interface into different languages */

};

#endif
