#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <QObject>
#define DEC_PRINT_SLOT(requestName) virtual void print ## requestName (QSharedPointer<GostCrypt::Core::requestName ## Response> r)

// default values for creating a volume
#define DEFAULT_ALGORITHM "Gost Grasshopper"
#define DEFAULT_KDF "Whirlpool"
#define DEFAULT_SIZE 10485760 // 10Mio
#define DEFAULT_OUTER_SIZE 1.0
#define DEFAULT_INNER_SIZE 0.7

/**
 * @brief Abstract class inherited by CmdLineInterface and GraphicInterface
 * Centralizes common functionality
 */
class UserInterface : public QObject
{
    Q_OBJECT
 public:
    /**
     * @brief Class constructor
     *
     * @param parent Parent object
     */
    explicit UserInterface(QObject* parent = nullptr) : QObject(parent) {}
    /**
     * @brief Creates the graphical interface or commandline interface and sends all the necessary objects to it in context
     *
     * @param argc The number of arguments in the main program
     * @param argv Arguments of the main program
     * @return int Returns what the "exec" method returns at the end of the interface execution
     */
    virtual int start(int argc, char** argv) = 0; //TODO : never used
    /**
     * @brief Converts A size in bytes into a formatted character string (or not formatted)
     *
     * @param sizeInByte Size in bytes to convert
     * @param withFontColor This boolean allows you to specify whether you want an output formatted in HTML (true) or not (false)
     * @return QString Character string of the size converted to the desired format
     */
    static QString formatSize(quint64 sizeInByte, bool withFontColor);
    static quint64 parseSize(QString s, bool* ok);
};

#endif // USERINTERFACE_H
