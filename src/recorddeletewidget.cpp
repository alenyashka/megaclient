#include "recorddeletewidget.h"

RecordDeleteWidget::RecordDeleteWidget()
{
    yesButton = new QPushButton(tr("Yes"));
    yesButton->setStatusTip(tr("Finish the operation"));
    yesButton->setIcon(QIcon(":/images/ok.png"));
    connect(yesButton, SIGNAL(clicked()), this, SLOT(yes()));

    noButton = new QPushButton(tr("No"));
    noButton->setStatusTip(tr("Cancel the operation"));
    noButton->setIcon(QIcon(":/images/cancel.png"));
    connect(noButton, SIGNAL(clicked()), this, SLOT(cancel()));

    backButton = new QPushButton(tr("Back"));
    backButton->setStatusTip(tr("Back to record's list"));
    backButton->setIcon(QIcon(":images/back.png"));
    connect(backButton, SIGNAL(clicked()), this, SLOT(cancel()));

    backToTableListButton = new QPushButton(tr("Back"));
    backToTableListButton->setStatusTip(tr("Back to table's list"));
    backToTableListButton->setIcon(QIcon(":images/back.png"));
    connect(backToTableListButton, SIGNAL(clicked()),
            this, SLOT(backToTableList()));

    messageLabel = new QLabel();
    errorLabel = new QLabel();

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(yesButton);
    buttonLayout->addWidget(noButton);
    buttonLayout->addWidget(backButton);
    buttonLayout->addWidget(backToTableListButton);
    buttonLayout->addSpacerItem(new QSpacerItem(220, 20, QSizePolicy::Fixed,
                                                QSizePolicy::Fixed));
    buttonLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum,
                                                QSizePolicy::Expanding));

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(messageLabel);
    leftLayout->addWidget(errorLabel);
    leftLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum,
                                              QSizePolicy::Expanding));

    groupBox = new QGroupBox(tr("Delete record"));
    groupBox->setLayout(leftLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(groupBox);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void RecordDeleteWidget::show(const QString &name,
                              const QString &title)
{
    this->tableName = name;
    this->recordTitle = title;
    MainWindow::Instance()->setCentralWidget(this);
    MainWindow::Instance()->setStatusLabelText("");
    messageLabel->setText(tr("Now record %1 will be deleted from table %2.\n"
                             "This action can not be reverted.\n"
                             "Do you agree?").arg(title).arg(name));
    errorLabel->setVisible(false);
    backButton->setVisible(false);
    backToTableListButton->setVisible(false);
}

void RecordDeleteWidget::cancel()
{
    closeConnection();
}

void RecordDeleteWidget::yes()
{
    MegaTcpSocket *tcpSocket = MegaTcpSocket::Instance();
    connect(tcpSocket, SIGNAL(connected()),
            this, SLOT(sendRequest()));
    connect(tcpSocket, SIGNAL(readyRead()),
            this, SLOT(getResponse()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error()));
    connect(tcpSocket, SIGNAL(disconnected()),
            this, SLOT(connectionClosedByServer()));
    tcpSocket->abort();
    tcpSocket->connectToHost();
    yesButton->setEnabled(false);
    noButton->setEnabled(false);
    MainWindow::Instance()->setStatusLabelText(tr("Connecting to server..."));
    nextBlockSize = 0;
}

void RecordDeleteWidget::sendRequest()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << quint16(0);
    out << MegaProtocol::DEL_RECORD << this->tableName << this->recordTitle;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    MegaTcpSocket::Instance()->write(block);
}

void RecordDeleteWidget::getResponse()
{
    MegaTcpSocket *tcpSocket = MegaTcpSocket::Instance();
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_5);
    uint status;
    in >> status;
    if (status == MegaProtocol::OK)
    {
        in >> nextBlockSize;
        closeConnection();
        MainWindow::Instance()->setStatusLabelText(
                tr("Record deleted successfully"));
    }
    else
    {
        uint err;
        in >> err;
        switch (err)
        {
            case MegaProtocol::RECORD_DELETED:
                showError(tr("This record is already deleted"));
                tcpSocket->abort();
                nextBlockSize = 0;
                yesButton->setVisible(false);
                noButton->setVisible(false);
                backToTableListButton->setVisible(false);
                backButton->setVisible(true);
                break;
            case MegaProtocol::TABLE_DELETED:
                showError(tr("Table with this table is already deleted"));
                tcpSocket->abort();
                yesButton->setVisible(false);
                noButton->setVisible(false);
                backButton->setVisible(false);
                backToTableListButton->setVisible(true);
                break;
            default:
                break;
        }
        in >> nextBlockSize;
    }
}

void RecordDeleteWidget::connectionClosedByServer()
{
   if (nextBlockSize != 0xFFFF)
    {
        MainWindow::Instance()->setStatusLabelText(
                tr("Error: Connection closed by server"));
    }
}

void RecordDeleteWidget::error()
{
    MainWindow::Instance()->setStatusLabelText(
            MegaTcpSocket::Instance()->errorString());
    closeConnection();
}

void RecordDeleteWidget::showError(const QString &error)
{
    errorLabel->setText(QString("<font color=red>").append(error).append("</font>"));
    errorLabel->setVisible(true);
}

void RecordDeleteWidget::closeConnection()
{
    yesButton->setEnabled(true);
    noButton->setEnabled(true);
    MegaTcpSocket *tcpSocket = MegaTcpSocket::Instance();
    disconnect(tcpSocket, SIGNAL(connected()),
               this, SLOT(sendRequest()));
    disconnect(tcpSocket, SIGNAL(readyRead()),
               this, SLOT(getResponse()));
    disconnect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
               this, SLOT(error()));
    disconnect(tcpSocket, SIGNAL(disconnected()),
               this, SLOT(connectionClosedByServer()));
    tcpSocket->abort();
    RecordListWidget::Instance()->show(this->tableName);
}

void RecordDeleteWidget::backToTableList()
{
    TableListWidget::Instance()->show();
}
