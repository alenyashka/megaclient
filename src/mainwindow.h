#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "singleton.h"
#include "megaguard.h"
#include "megaconnector.h"
#include "tablelistwidget.h"

class TableListWidget;
class MainWindow : public QMainWindow, public Singleton<MainWindow>
{
    Q_OBJECT
protected:
    MainWindow(QWidget *parent = 0);
    friend class Singleton<MainWindow>;
    void closeEvent(QCloseEvent *event);
public:
    void writeSettings();
    void show();
public slots:
    void setStatusLabelText(QString);
    void state(QTcpSocket::SocketState);
private:
    void readSettings();
    QLabel *statusLabel;
    QLabel *iconLabel;
    TableListWidget *tableListWidget;
};

#endif // MAINWINDOW_H
