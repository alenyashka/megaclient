#ifndef MEGATCPSOCKET_H
#define MEGATCPSOCKET_H

#include <QTcpSocket>

class MegaTcpSocket : public QTcpSocket
{
    Q_OBJECT
protected:
    static MegaTcpSocket *_self;
    static int _refcount;
    MegaTcpSocket();
    virtual ~MegaTcpSocket()
    {
        _self = NULL;
    }
public:
    static MegaTcpSocket *Instance()
    {
        if (!_self) _self = new MegaTcpSocket();
        return _self;
    }
    void FreeInst()
    {
        if (--_refcount == 0)
        {
            delete this;
        }
    }
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
