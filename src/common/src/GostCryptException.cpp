/**
 * @file GostCryptException.cpp
 * @author badbouille
 * @date 16/11/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "GostCryptException.h"

#include <sstream>
#include <cstring>

GostCrypt::GostCryptException::GostCryptException(const char * message,
                                                  const char *function,
                                                  const char *filename,
                                                  uint32_t line)
{
    std::stringstream output;
    msg = message;
    func = function;
    file = filename;
    l = line;

    /* Building final message */
    output << exceptionName;
    output << " in " << file;
    output << "::" << func;
    output << "::" << l;
    output << " " << msg;
    compiledmessage = output.str();
}

GostCrypt::GostCryptException::GostCryptException(std::string message,
                                                  const char *function,
                                                  const char *filename,
                                                  uint32_t line)
{
    std::stringstream output;
    msg = std::move(message);
    func = function;
    file = filename;
    l = line;

    /* Building final message */
    output << exceptionName;
    output << " in " << file;
    output << "::" << func;
    output << "::" << l;
    output << " " << msg;
    compiledmessage = output.str();
}
