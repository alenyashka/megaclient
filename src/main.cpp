#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator appTranslator;
    appTranslator.load("MegaClient_" + QLocale::system().name(), qApp->applicationDirPath());
    qApp->installTranslator(&appTranslator);
    MainWindow *w = MainWindow::Instance();
    w->show();
    return a.exec();
}
