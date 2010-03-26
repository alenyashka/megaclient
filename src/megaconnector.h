#ifndef MEGACONNECTOR_H
#define MEGACONNECTOR_H

#include <QThread>
#include <QTcpSocket>
#include <QTimer>
#include <QVariant>
#include "singleton.h"

class MegaConnector: public QThread, public Singleton<MegaConnector>
{
    Q_OBJECT
protected:
    MegaConnector();
    friend class Singleton<MegaConnector>;
public:
    void run();
    void setHost(const QString&);
    void setPort(const quint16&);
    void setTimeout(const int&);
    void setRefreshRate(const int&);
    QString getHost();
    quint16 getPort();
    int getTimeout();
    int getRefreshRate();
    void write(QByteArray);
private slots:
    void update();
    void error();
    void getResponse();
    void tryToConnect();
signals:
    void refresh();
    void statusMsg(QString);
    void state(QTcpSocket::SocketState);
    void response(QByteArray);
private:
    QTcpSocket *tcpSocket;
    QTimer *updateTimer;
    QTimer *connectTimer;
    QString host;
    quint16 port;
    quint16 attempt;
    int timeout;
    int refreshRate;
    quint16 nextBlockSize;
};

#endif // MEGACONNECTOR_H
