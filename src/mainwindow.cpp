#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    readSettings();
    setWindowTitle(tr("Mega Client"));
    statusBar = new QStatusBar();
    statusLabel = new QLabel();
    statusBar->addWidget(statusLabel);
    this->setStatusBar(statusBar);
}
void MainWindow::show()
{
    TableListWidget::Instance()->show();
    setVisible(true);
}

void MainWindow::setStatusLabelText(const QString &text)
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
    MegaTcpSocket *tcpSocket = MegaTcpSocket::Instance();
    tcpSocket->setHost(settings.value("Network/host", "localhost").toString());
    tcpSocket->setPort(settings.value("Network/port", 6178).toInt());
}

void MainWindow::writeSettings()
{
    QSettings settings("AlexAnis Inc.", "MegaClient");
    settings.beginGroup("Interface");
    settings.setValue("geometry", geometry());
    settings.endGroup();
    settings.beginGroup("Network");
    settings.setValue("host", MegaTcpSocket::Instance()->getHost());
    settings.setValue("port", MegaTcpSocket::Instance()->getPort());
    settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}
