#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QTcpSocket>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
private slots:
    void sendRequest();
    void connectToServer();
    void showMsgDialog();
    void error();

private:
    Ui::MainWindow *ui;
    void closeConnection();
    QTcpSocket tcpSocket;
    quint16 nextBlockSize;
};

#endif // MAINWINDOW_H
