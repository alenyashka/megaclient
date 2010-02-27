#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createMainForm();
    setWindowTitle(tr("Mega Client"));
    connect(&tcpSocket, SIGNAL(connected()), this, SLOT(sendRequest()));
    connect(&tcpSocket, SIGNAL(readyRead()), this, SLOT(showMsgDialog()));
    connect(&tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error()));
    readSettings();
}

void MainWindow::createMainForm()
{
    updateButton = new QPushButton(tr("Update"));
    updateButton->setShortcut(tr("Ctrl+R"));
    updateButton->setStatusTip(tr("Update list of tables"));
    updateButton->setIcon(QIcon(":/images/update.png"));
    connect(updateButton, SIGNAL(clicked()), this, SLOT(connectToServer()));

    settingsButton = new QPushButton(tr("Settings"));
    settingsButton->setStatusTip(tr("Edit settings of application"));
    settingsButton->setIcon(QIcon(":images/settings.png"));

    quitButton = new QPushButton(tr("Quit"));
    quitButton->setShortcut(tr("Ctrl+Q"));
    quitButton->setStatusTip(tr("Quit from application"));
    quitButton->setIcon(QIcon(":images/quit.png"));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

    viewTableButton = new QPushButton(tr("View table"));
    viewTableButton->setStatusTip(tr("View records of selected tables"));
    viewTableButton->setShortcut(tr("Ctrl+Alt+V"));
    viewTableButton->setIcon(QIcon(":images/view.png"));

    addTableButton = new QPushButton(tr("Add table"));
    addTableButton->setStatusTip(tr("Add tables to the list"));
    addTableButton->setShortcut(tr("Ctrl+Alt+A"));
    addTableButton->setIcon(QIcon(":images/add.png"));

    editTableButton = new QPushButton(tr("Edit table"));
    editTableButton->setStatusTip(tr("Edit current tabel"));
    editTableButton->setShortcut(tr("Ctrl+Alt+E"));
    editTableButton->setIcon(QIcon(":/images/edit.png"));

    delTableButton = new QPushButton(tr("Delete table"));
    delTableButton->setStatusTip(tr("Delete current tabel from the list"));
    delTableButton->setShortcut(tr("Ctrl+Alt+D"));
    delTableButton->setIcon(QIcon(":/images/del.png"));

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(viewTableButton);
    rightLayout->addWidget(addTableButton);
    rightLayout->addWidget(editTableButton);
    rightLayout->addWidget(delTableButton);
    rightLayout->addWidget(updateButton);
    rightLayout->addWidget(settingsButton);
    rightLayout->addWidget(quitButton);
    rightLayout->addSpacerItem(new QSpacerItem(20,40,QSizePolicy::Minimum,
                                               QSizePolicy::Expanding));

    titleLabel = new QLabel(tr("Table's list"));
    titleLabel->setFont(QFont("Arial", 18, 10, false));

    tableTableWidget = new QTableWidget(parentWidget());
    QHBoxLayout *centralLayout = new QHBoxLayout;
    centralLayout->addWidget(tableTableWidget);
    centralLayout->addLayout(rightLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(centralLayout);

    QWidget *centralWidget = new QWidget(parentWidget());
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    statusBar = new QStatusBar(parentWidget());
    this->setStatusBar(statusBar);
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
                                QRect(200, 200, 640, 480)).toRect();
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
