/**
 * @file UserInterface.h
 * @author seragonia
 * @date 20/01/2019
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <QObject>
#include <QVariant>
#define DEC_SEND_SLOT(requestName) virtual void send ## requestName (QVariant)

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
    virtual int start(int argc, char** argv) = 0;
    /**
     * @brief Converts A size in bytes into a formatted character string (or not formatted)
     *
     * @param sizeInByte Size in bytes to convert
     * @param withFontColor This boolean allows you to specify whether you want an output formatted in HTML (true) or not (false)
     * @return QString Character string of the size converted to the desired format
     */
    static QString formatSize(quint64 sizeInByte, bool withFontColor);
    static quint64 parseSize(QString s, bool* ok);

 private slots:  // NOLINT
    DEC_SEND_SLOT(CreateVolume) = 0;
    DEC_SEND_SLOT(MountVolume) = 0;
    DEC_SEND_SLOT(DismountVolume) = 0;
    DEC_SEND_SLOT(GetMountedVolumes) = 0;
    DEC_SEND_SLOT(GetEncryptionAlgorithms) = 0;
    DEC_SEND_SLOT(GetDerivationFunctions) = 0;
    DEC_SEND_SLOT(GetHostDevices) = 0;
    DEC_SEND_SLOT(CreateKeyFile) = 0;
    DEC_SEND_SLOT(ChangeVolumePassword) = 0;
    DEC_SEND_SLOT(BenchmarkAlgorithms) = 0;

    /**
     * @brief Update the progress for the corresponding request in the user interface
     */
    virtual void printProgressUpdate(quint32 requestId, qreal progress) = 0;

};

#endif // USERINTERFACE_H
