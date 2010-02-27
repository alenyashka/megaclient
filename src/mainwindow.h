#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QTcpSocket>

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
    void createMainForm();
    QString host;
    qint16 port;
    QTcpSocket tcpSocket;
    quint16 nextBlockSize;
    QPushButton *updateButton;
    QPushButton *settingsButton;
    QPushButton *quitButton;
    QPushButton *viewTableButton;
    QPushButton *addTableButton;
    QPushButton *editTableButton;
    QPushButton *delTableButton;
    QStatusBar *statusBar;
    QTableWidget *tableTableWidget;
    QLabel *titleLabel;
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
