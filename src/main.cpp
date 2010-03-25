#include "mainwindow.h"
#include "apppath.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator appTranslator;
    appTranslator.load("megaclient_" + QLocale::system().name(),
                       AppPath::getL10nPath());
    qApp->installTranslator(&appTranslator);
    MainWindow *w = MainWindow::Instance();
    w->show();
    return a.exec();
}
