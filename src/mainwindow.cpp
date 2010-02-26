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
    readSettings();
}

void MainWindow::connectToServer()
{
    tcpSocket.connectToHost(host, port);
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

void MainWindow::readSettings()
{
    QSettings settings("AlexAnis Inc.", "MegaClient");
    QRect rect = settings.value("Interface/geometry",
                                QRect(200, 200, 300, 103)).toRect();
    move(rect.topLeft());
    resize(rect.size());
    host = settings.value("Network/host", "localhost").toString();
    port = settings.value("Network/port", 6178).toInt();
}

void MainWindow::writeSettings()
{
    QSettings settings("AlexAnis Inc.", "MegaClient");
    settings.beginGroup("Interface");
    settings.setValue("geometry", geometry());
    settings.endGroup();
    settings.beginGroup("Network");
    settings.setValue("host", host);
    settings.setValue("port", port);
    settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}
