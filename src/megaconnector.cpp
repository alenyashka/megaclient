#include "megaconnector.h"

MegaConnector::MegaConnector()
{
    attempt = 1;
    tcpSocket = new QTcpSocket;
    updateTimer = NULL;
    connectTimer = NULL;
}

void MegaConnector::run()
{
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    connect(tcpSocket, SIGNAL(connected()),
            this, SLOT(update()), Qt::DirectConnection);
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error()), Qt::DirectConnection);
    connect(tcpSocket, SIGNAL(readyRead()),
            this, SLOT(getResponse()), Qt::DirectConnection);
    emit state(tcpSocket->state());
    emit statusMsg(tr("Attempt %1: Connecting to server...").arg(attempt));
    attempt++;
    tcpSocket->connectToHost(host, port);
    exec();
}

void MegaConnector::update()
{
    if ((connectTimer != NULL) && (connectTimer->isActive()))
    {
        connectTimer->stop();
        free(connectTimer);
        connectTimer = NULL;
    }
    updateTimer = new QTimer;
    connect(updateTimer, SIGNAL(timeout()), this, SIGNAL(refresh()));
    updateTimer->setInterval(refreshRate * 1000);
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error()));
    emit statusMsg(tr("Connection established"));
    attempt = 1;
    emit state(QTcpSocket::ConnectedState);
    emit refresh();
    updateTimer->start();
}

void MegaConnector::error()
{
    emit refresh();
    emit statusMsg(tr("Connection refused"));
    emit state(QTcpSocket::UnconnectedState);
    if ((updateTimer != NULL) && (updateTimer->isActive()))
    {
        updateTimer->stop();
        free(updateTimer);
        updateTimer = NULL;
    }
    connectTimer = new QTimer;
    connect(connectTimer, SIGNAL(timeout()), this, SLOT(tryToConnect()));
    disconnect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error()));
    connectTimer->setInterval(timeout * 1000);
    connectTimer->start();
}

void MegaConnector::tryToConnect()
{
    emit statusMsg(tr("Attempt %1: Connecting to server...").arg(attempt));
    attempt++;
    tcpSocket->close();
    tcpSocket->connectToHost(host, port);
}

void MegaConnector::setHost(const QString &host)
{
    this->host = host;
}

void MegaConnector::setPort(const quint16 &port)
{
    this->port = port;
}

void MegaConnector::setTimeout(const int &timeout)
{
    this->timeout = timeout;
}

void MegaConnector::setRefreshRate(const int &rr)
{
    this->refreshRate = rr;
}

QString MegaConnector::getHost()
{
    return host;
}

quint16 MegaConnector::getPort()
{
    return port;
}

int MegaConnector::getTimeout()
{
    return timeout;
}

int MegaConnector::getRefreshRate()
{
    return refreshRate;
}

void MegaConnector::write(QByteArray block)
{
    if (tcpSocket->isOpen())
    {
        tcpSocket->write(block);
        tcpSocket->flush();
        tcpSocket->seek(0);
    }
}

void MegaConnector::getResponse()
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_5);
    QByteArray block = in.device()->readAll();
    emit response(block);
}
