#include <QApplication>
#include "winmanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    WinManager winManager;
    winManager.showDashboard();

    return app.exec();
}
