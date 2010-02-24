#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->TestButton, SIGNAL(clicked()), this, SLOT(connectToServer()));
    connect(&tcpSocket, SIGNAL(connected()), this, SLOT(sendRequest()));
    connect(&tcpSocket, SIGNAL(readyRead()), this, SLOT(showMsgDialog()));
    connect(&tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error()));
}

void MainWindow::connectToServer()
{
    tcpSocket.connectToHost("localhost", 6178);
    nextBlockSize = 0;
}

void MainWindow::showMsgDialog()
{
    qDebug() << "Enter: MainWindows::showMsgDialog";
    QDataStream in(&tcpSocket);
    in.setVersion(QDataStream::Qt_4_5);
    forever
    {
        if (nextBlockSize == 0)
        {
            if (tcpSocket.bytesAvailable() < sizeof(qint16)) break;
            in >> nextBlockSize;
        }
        if (nextBlockSize == 0xFFFF)
        {
            closeConnection();
            break;
        }
        if (tcpSocket.bytesAvailable() < nextBlockSize) break;
        QString message;
        in >> message;
        QMessageBox msgbox;
        msgbox.setText(message);
        msgbox.exec();
    }
    qDebug() << "Leave: MainWindows::showMsgDialog";
}

void MainWindow::closeConnection()
{
    tcpSocket.close();
}

void MainWindow::sendRequest()
{
    qDebug() << "Enter: MainWindow::sendRequest";
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << qint16(0) << "Hello";
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(qint16));
    tcpSocket.write(block);
    qDebug() << "Leave: MainWindow::sendRequest";
}

void MainWindow::error()
{
    qDebug() << tcpSocket.errorString();
    closeConnection();
}
