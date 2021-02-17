/**
 * @file main.cpp
 * @author seragonia
 * @date 20/01/2019
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <ForkableCore.h>
#include "GraphicInterface.h"

const char *g_prog_path;

int main(int argc, char* argv[])
{
    g_prog_path = argv[0];
    if (std::string(argv[0]) == "api") {
        return ForkableCore_api_handler(argc, argv);
    }
    qSetMessagePattern("%{appname}: %{message}");
    MyGuiApplication app(argc, argv);
    GraphicInterface gi(&app);
    return gi.start(argc, argv);
}
