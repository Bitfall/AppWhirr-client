#include <QtGui/QApplication>
#include <QMetaType>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    qRegisterMetaType<BaseNetworkEntity*>("BaseNetworkEntity*");
    qRegisterMetaType< QList<BaseNetworkEntity*> >("QList<BaseNetworkEntity*>");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
