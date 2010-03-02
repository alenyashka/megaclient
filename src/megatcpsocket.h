#ifndef MEGATCPSOCKET_H
#define MEGATCPSOCKET_H

#include <QTcpSocket>
#include "singleton.h"

class MegaTcpSocket : public QTcpSocket, public Singleton<MegaTcpSocket>
{
    Q_OBJECT
protected:
    friend class Singleton<MegaTcpSocket>;
public:
    void connectToHost();
    void setHost(const QString&);
    void setPort(const qint16&);
    QString getHost();
    qint16 getPort();
private:
    QString host;
    qint16 port;
};

#endif // MEGATCPSOCKET_H
