#include "tableadedwidget.h"

TableAdEdWidget::TableAdEdWidget()
{
    okButton = new QPushButton(tr("Ok"));
    okButton->setStatusTip(tr("Finish the operation"));
    okButton->setIcon(QIcon(":/images/ok.png"));
    connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));

    cancelButton = new QPushButton(tr("Cancel"));
    cancelButton->setStatusTip(tr("Cancel the operation"));
    cancelButton->setIcon(QIcon(":/images/cancel.png"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

    titleLabel = new QLabel();
    titleLabel->setFont(QFont("AlArabia", 20, 50, false));
    nameLabel = new QLabel(tr("Name:"));
    commentLabel = new QLabel(tr("Comment:"));
    errorLabel = new QLabel();
    errorLabel->setFont(QFont("AlArabia", 16, 40, false));
    errorLabel->setVisible(false);

    nameLineEdit = new QLineEdit();
    connect(nameLineEdit, SIGNAL(textEdited(QString)),
            this, SLOT(hideError()));
    commentTextEdit = new QTextEdit();

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(nameLabel);
    leftLayout->addWidget(nameLineEdit);
    leftLayout->addWidget(commentLabel);
    leftLayout->addWidget(commentTextEdit);
    leftLayout->addWidget(errorLabel);

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum,
                                                QSizePolicy::Expanding));
    QHBoxLayout *centralLayout = new QHBoxLayout;
    centralLayout->addLayout(leftLayout);
    centralLayout->addLayout(buttonLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(centralLayout);

    setLayout(mainLayout);
}

void TableAdEdWidget::show(const QString &name,
                           const QString &comment)
{
    nameLineEdit->setText(name);
    commentTextEdit->setText(comment);
    errorLabel->setEnabled(true);
    //nameLineEdit->selectAll();
    nameLineEdit->setFocus();
    if (name == "")
    {
        titleLabel->setText(tr("Add table"));
        action = this->ADD;
    }
    else
    {
        titleLabel->setText(tr("Edit table"));
        action = this->EDIT;
    }
    MainWindow::Instance()->setCentralWidget(this);
    MainWindow::Instance()->setStatusLabelText("");
}

void TableAdEdWidget::cancel()
{
    TableListWidget::Instance()->show();
}

bool TableAdEdWidget::isError()
{
    if (nameLineEdit->text() == "")
    {
        showError(tr("You should enter name of the table"));
        nameLineEdit->setFocus();
        return true;
    }
    return false;
}

void TableAdEdWidget::showError(const QString &error)
{
    errorLabel->setText(QString("<font color=red>").append(error).append("</font>"));
    errorLabel->setVisible(true);
}

void TableAdEdWidget::ok()
{
    if (isError()) return;
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
    okButton->setEnabled(false);
    MainWindow::Instance()->setStatusLabelText(tr("Connecting to server..."));
    nextBlockSize = 0;
}

void TableAdEdWidget::sendRequest()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << quint16(0);
    if (action == this->ADD)
    {
        out << MegaProtocol::ADD_TABLE << nameLineEdit->text() <<
                commentTextEdit->toPlainText();
    }
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    MegaTcpSocket::Instance()->write(block);
}

void TableAdEdWidget::getResponse()
{
    MegaTcpSocket *tcpSocket = MegaTcpSocket::Instance();
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_5);

    if (nextBlockSize == 0)
    {
        if (tcpSocket->bytesAvailable() < sizeof(quint16)) return;
        in >> nextBlockSize;
    }
    if (nextBlockSize == 0xFFFF)
    {
        closeConnection();
        MainWindow::Instance()->setStatusLabelText(
                tr("Table added successfully"));
        return;
    }
    QString err;
    in >> err;
    showError(err);
    nameLineEdit->selectAll();
    nameLineEdit->setFocus();
    tcpSocket->abort();
    okButton->setEnabled(true);
    nextBlockSize = 0;
}

void TableAdEdWidget::error()
{
    MainWindow::Instance()->setStatusLabelText(
            MegaTcpSocket::Instance()->errorString());
    closeConnection();
}

void TableAdEdWidget::connectionClosedByServer()
{
    if (nextBlockSize != 0xFFFF)
    {
        MainWindow::Instance()->setStatusLabelText(
                tr("Error: Connection closed by server"));
    }
}

void TableAdEdWidget::closeConnection()
{
    okButton->setEnabled(true);
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
    TableListWidget::Instance()->show();
}

void TableAdEdWidget::hideError()
{
    errorLabel->clear();
    errorLabel->setVisible(false);
}
