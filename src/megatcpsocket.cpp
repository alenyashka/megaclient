#include "megatcpsocket.h"

void MegaTcpSocket::setHost(const QString &host)
{
    this->host = host;
}

void MegaTcpSocket::setPort(const qint16 &port)
{
    this->port = port;
}

QString MegaTcpSocket::getHost()
{
    return host;
}

qint16 MegaTcpSocket::getPort()
{
    return port;
}

void MegaTcpSocket::connectToHost()
{

    QTcpSocket::connectToHost(host, port);
}
