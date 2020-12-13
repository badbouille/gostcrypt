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
     * @brief Retrieve a pointer on the graphical interface object
     *
     * @param gi Pointer to the currently instantiated QML interface
     */
    void setGI(GraphicInterface* gi) { mGI = gi; }

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

 private:
    MyGuiApplication* mApp; /**< Object inheriting the QApplication, the interface works thanks to this object */
    QQmlApplicationEngine mEngine; /**< QML rendering engine used to render the display, contains the main context */
    UserSettings mSettings; /**< User preferences */
    DragWindowProvider mDrag; /**< Class that allows you to move the window (which is borderless) */
    TranslationApp mTranslation; /**< Class to translate the interface into different languages */

};

#endif
