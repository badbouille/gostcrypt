/**
 * @file main.cpp
 * @author seragonia
 * @date 20/01/2019
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <Core.h>
#include "GraphicInterface.h"

int main(int argc, char* argv[])
{
    /* Interception of api calls */
    if (GostCrypt::Core::main_api_handler(argc, argv) != 0) {
        return 1; // no exception needed but can't continue
    }

    qSetMessagePattern("%{appname}: %{message}");
    MyGuiApplication app(argc, argv);
    GraphicInterface gi(&app);
    return gi.start(argc, argv);
}
