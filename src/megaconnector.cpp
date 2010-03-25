#include "megaconnector.h"

MegaConnector::MegaConnector()
{
    attempt = 1;
    tcpSocket = new QTcpSocket;
}

void MegaConnector::run()
{
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
    mytimer = new QTimer;
    connect(mytimer, SIGNAL(timeout()), this, SIGNAL(refresh()));
    mytimer->setInterval(/*refreshRate * */1000);
    emit statusMsg(tr("Connection established"));
    attempt = 1;
    emit state(tcpSocket->state());
    emit refresh();
    mytimer->start();
}

void MegaConnector::error()
{
    emit statusMsg(tr("Error: %1").arg(tcpSocket->errorString()));
    emit state(tcpSocket->state());
    sleep(timeout);
    emit statusMsg(tr("Attempt %1: Connecting to server...").arg(attempt));
    tcpSocket->close();
    tcpSocket->connectToHost(host, port);
    attempt++;
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
