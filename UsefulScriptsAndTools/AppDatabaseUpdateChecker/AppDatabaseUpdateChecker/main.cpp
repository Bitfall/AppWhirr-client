#include <QtGui/QApplication>
#include "appdatabaseupdatecheckermainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AppDatabaseUpdateCheckerMainWindow w;
    w.show();

    return a.exec();
}
