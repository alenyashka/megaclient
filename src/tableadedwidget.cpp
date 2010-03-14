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

    backButton = new QPushButton(tr("Back"));
    backButton->setStatusTip(tr("Back to tables list"));
    backButton->setIcon(QIcon(":images/back.png"));
    connect(backButton, SIGNAL(clicked()), this, SLOT(cancel()));

    nameLabel = new QLabel(tr("Name:"));
    commentLabel = new QLabel(tr("Comment:"));
    errorLabel = new QLabel();
    errorLabel->setVisible(false);

    nameLineEdit = new QLineEdit();
    connect(nameLineEdit, SIGNAL(textEdited(QString)),
            this, SLOT(hideError()));
    commentTextEdit = new QTextEdit();

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(nameLabel);
    leftLayout->addWidget(nameLineEdit);
    leftLayout->addWidget(errorLabel);
    leftLayout->addWidget(commentLabel);
    leftLayout->addWidget(commentTextEdit);

    groupBox = new QGroupBox();
    groupBox->setLayout(leftLayout);

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(backButton);
    buttonLayout->addSpacerItem(new QSpacerItem(220, 20, QSizePolicy::Fixed,
                                                QSizePolicy::Fixed));
    buttonLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum,
                                                QSizePolicy::Expanding));

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(groupBox);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void TableAdEdWidget::show(const Mode &mode,
                           const QString &name,
                           const QString &comment)
{
    this->oldName = name;
    nameLineEdit->setText(name);
    commentTextEdit->setText(comment);
    nameLineEdit->setFocus();
    this->mode = mode;
    switch (mode)
    {
        case ViewMode:
            readOnlyMode(true);
            groupBox->setTitle(tr("View table"));
            break;
        case AddMode:
            readOnlyMode(false);
            groupBox->setTitle(tr("Add table"));
            break;
        case EditMode:
            readOnlyMode(false);
            groupBox->setTitle(tr("Edit table"));
            break;
    }
    MainWindow::Instance()->setCentralWidget(this);
    MainWindow::Instance()->setStatusLabelText("");
}

void TableAdEdWidget::readOnlyMode(const bool &state)
{
    nameLineEdit->setReadOnly(state);
    commentTextEdit->setReadOnly(state);
    backButton->setVisible(state);
    okButton->setVisible(!state);
    cancelButton->setVisible(!state);
}

void TableAdEdWidget::cancel()
{
    closeConnection();
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
    switch (mode)
    {
        case AddMode:
            out << MegaProtocol::ADD_TABLE;
            break;
        case EditMode:
            out << MegaProtocol::EDIT_TABLE << oldName;
            break;
        default:
            break;
    }
    out << nameLineEdit->text() << commentTextEdit->toPlainText();
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
        QString success;
        switch (mode)
        {
            case AddMode:
                success = tr("Table added successfully");
                break;
            case EditMode:
                success = tr("Table edited successfully");
                break;
            default:
                break;
        }
        MainWindow::Instance()->setStatusLabelText(success);
        return;
    }
    uint err;
    in >> err;
    switch (err)
    {
        case MegaProtocol::TABLE_EXIST:
            showError(tr("Table with this name already exist"));
            nameLineEdit->selectAll();
            nameLineEdit->setFocus();
            tcpSocket->abort();
            okButton->setEnabled(true);
            nextBlockSize = 0;
            break;
        case MegaProtocol::TABLE_DELETED:
            showError(tr("This table is already deleted"));
            nameLineEdit->selectAll();
            nameLineEdit->setFocus();
            tcpSocket->abort();
            okButton->setVisible(false);
            nextBlockSize = 0;
        default:
            break;
    }
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
