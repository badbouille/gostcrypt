/**
 * @file main.cpp
 * @author seragonia
 * @date 20/01/2019
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <ForkableCore.h>
#include "GraphicInterface.h"

int main(int argc, char* argv[])
{
    /* Interception of api calls */
    ForkableCore_api_handler(argc, argv);

    qSetMessagePattern("%{appname}: %{message}");
    MyGuiApplication app(argc, argv);
    GraphicInterface gi(&app);
    return gi.start(argc, argv);
}
