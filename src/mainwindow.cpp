#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    readSettings();
    setWindowTitle(tr("Mega Client"));

    statusLabel = new QLabel();
    iconLabel = new QLabel();
    iconLabel->setToolTip(tr("Connection status"));

    statusBar()->addWidget(statusLabel);
    statusBar()->addPermanentWidget(iconLabel);

    MegaConnector *con = MegaConnector::Instance();
    connect(con, SIGNAL(statusMsg(QString)),
            this, SLOT(setStatusLabelText(QString)), Qt::DirectConnection);
    qRegisterMetaType<QTcpSocket::SocketState>("QTcpSocket::SocketState");
    connect(con, SIGNAL(state(QTcpSocket::SocketState)),
            this, SLOT(state(QTcpSocket::SocketState)));
}

void MainWindow::show()
{
    TableListWidget::Instance()->show();
    setVisible(true);
    MegaConnector::Instance()->start();
}

void MainWindow::setStatusLabelText(QString text)
{
    statusLabel->setText(text);
}

void MainWindow::readSettings()
{
    QSettings settings("AlexAnis Inc.", "MegaClient");
    QRect rect = settings.value("Interface/geometry",
                                QRect(200, 200, 640, 480)).toRect();
    move(rect.topLeft());
    resize(rect.size());
    MegaConnector *mc = MegaConnector::Instance();
    mc->setHost(settings.value("Network/host", "localhost").toString());
    mc->setPort(settings.value("Network/port", 6178).toInt());
    mc->setTimeout(settings.value("Network/timeout", 5).toInt());
    mc->setRefreshRate(settings.value("Network/refreshRate", 5).toInt());
    MegaGuard::Instance()->setPassword(
            settings.value("Privileges/password", "").toString());
}

void MainWindow::writeSettings()
{
    QSettings settings("AlexAnis Inc.", "MegaClient");
    settings.beginGroup("Interface");
    settings.setValue("geometry", geometry());
    settings.endGroup();
    settings.beginGroup("Network");
    MegaConnector *mc = MegaConnector::Instance();
    settings.setValue("host", mc->getHost());
    settings.setValue("port", mc->getPort());
    settings.setValue("timeout", mc->getTimeout());
    settings.setValue("refreshRate", mc->getRefreshRate());
    settings.endGroup();
    settings.beginGroup("Privileges");
    settings.setValue("password", MegaGuard::Instance()->getPassword());
    settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::state(QTcpSocket::SocketState s)
{
    QImage *image;
    switch (s)
    {
        case QTcpSocket::ConnectedState:
            image = new QImage(":/images/online.png");
            emit update();
            break;
        default:
            image = new QImage(":/images/offline.png");
            break;
    }
    iconLabel->setPixmap(QPixmap::fromImage(*image));
}
