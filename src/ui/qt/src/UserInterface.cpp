/**
 * @file UserInterface.cpp
 * @author seragonia
 * @date 20/01/2019
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "UserInterface.h"

/*! converts byte to MB, GB, KB */
QString UserInterface::formatSize(quint64 sizeInByte, bool withFontColor)
{
    if (sizeInByte < 1024) return ((withFontColor) ? QString("<font color=#6e9f45>") : QString(""))
                                      + QString::number(sizeInByte)
                                      + ((withFontColor) ? QString("</font>") : QString(""))
                                      + QString(" B");

    else if (sizeInByte < 1048576) return ((withFontColor) ? QString("<font color=#6e9f45>") :
                                               QString(""))
                                              + QString::number((float)sizeInByte / (float)1024, 'f', 1)
                                              + ((withFontColor) ? QString("</font>") : QString(""))
                                              + QString(" KB");

    else if (sizeInByte < 1073741824) return ((withFontColor) ? QString("<font color=#6e9f45>") :
                QString(""))
                + QString::number((float)sizeInByte / (float)1048576, 'f', 1)
                + ((withFontColor) ? QString("</font>") : QString(""))
                + QString(" MB");

    else return ((withFontColor) ? QString("<font color=#6e9f45>") : QString(""))
                    + QString::number((float)sizeInByte / (float)1073741824, 'f', 1)
                    + ((withFontColor) ? QString("</font>") : QString(""))
                    + QString(" GB");
}

quint64 UserInterface::parseSize(QString s, bool* ok)
{
    if (ok)
    {
        *ok = true;
    }

    if (s.at(s.size() - 1) == 'B')
    {
        s.chop(1);
    }

    if (s.at(s.size() - 1).isNumber())
    {
        return s.toInt();
    }
    if (s.at(s.size() - 1) == 'K')
    {
        s.chop(1);
        return s.toInt() * 1024;
    }
    if (s.at(s.size() - 1) == 'M')
    {
        s.chop(1);
        return s.toInt() * 1024 * 1024;
    }
    if (s.at(s.size() - 1) == 'G')
    {
        s.chop(1);
        return s.toInt() * 1024 * 1024 * 1024;
    }
    if (ok)
    {
        *ok = false;
    }
    return 0;
}
