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
    void closeConnection();
    void readSettings();
    void writeSettings();
    QString host;
    qint16 port;
    Ui::MainWindow *ui;
    QTcpSocket tcpSocket;
    quint16 nextBlockSize;
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
