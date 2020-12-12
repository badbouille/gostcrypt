#include "GraphicInterface.h"
#include "CmdLineInterface.h"

int main(int argc, char* argv[])
{
    qSetMessagePattern("%{appname}: %{message}");
    MyGuiApplication app(argc, argv);
    GraphicInterface gi(&app);
    return gi.start(argc, argv);
}
