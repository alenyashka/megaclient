#include "recordadedview.h"

RecordAdEdView::RecordAdEdView()
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
    backButton->setStatusTip(tr("Back to record's list"));
    backButton->setIcon(QIcon(":/images/back.png"));
    connect(backButton, SIGNAL(clicked()), this, SLOT(cancel()));

    backToTableListButton = new QPushButton(tr("Back"));
    backToTableListButton->setStatusTip(tr("Back to table's list"));
    backToTableListButton->setIcon(QIcon(":/images/back.png"));
    connect(backToTableListButton, SIGNAL(clicked()),
            this, SLOT(backToTableList()));

    titleLabel = new QLabel(tr("Title:"));
    errorTitleLabel = new QLabel();
    errorTitleLabel->setVisible(false);
    commentLabel = new QLabel(tr("Comment:"));
    readOnlyLabel = new QLabel(tr("Read only:"));
    typeLabel = new QLabel(tr("Type:"));
    valueLabel = new QLabel(tr("Value:"));

    titleLineEdit = new QLineEdit();
    connect(titleLineEdit, SIGNAL(textEdited(QString)),
            this, SLOT(hideError()));
    commentTextEdit = new QTextEdit();
    readOnlyCheckBox = new QCheckBox();
    typeComboBox = new QComboBox();
    typeComboBox->addItem("Double", (uint) QVariant::Double);
    typeComboBox->addItem("Int", (uint) QVariant::Int);
    typeComboBox->addItem("String", (uint) QVariant::String);
    valueLineEdit = new QLineEdit();

    QVBoxLayout *titleLayout = new QVBoxLayout;
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(titleLineEdit);
    titleLayout->addWidget(errorTitleLabel);

    QVBoxLayout *commentLayout = new QVBoxLayout;
    commentLayout->addWidget(commentLabel);
    commentLayout->addWidget(commentTextEdit);

    QHBoxLayout *readOnlyLayout = new QHBoxLayout;
    readOnlyLayout->addWidget(readOnlyLabel);
    readOnlyLayout->addWidget(readOnlyCheckBox);
    readOnlyLayout->addSpacerItem(new QSpacerItem(40, 20,
                                                  QSizePolicy::Expanding,
                                                  QSizePolicy::Minimum));

    QVBoxLayout *typeLayout = new QVBoxLayout;
    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(typeComboBox);

    QVBoxLayout *valueLayout = new QVBoxLayout;
    valueLayout->addWidget(valueLabel);
    valueLayout->addWidget(valueLineEdit);

    QVBoxLayout *groupBoxLayout = new QVBoxLayout;
    groupBoxLayout->addLayout(titleLayout);
    groupBoxLayout->addLayout(commentLayout);
    groupBoxLayout->addLayout(readOnlyLayout);
    groupBoxLayout->addLayout(typeLayout);
    groupBoxLayout->addLayout(valueLayout);

    groupBox = new QGroupBox();
    groupBox->setLayout(groupBoxLayout);

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(backButton);
    buttonLayout->addWidget(backToTableListButton);
    buttonLayout->addSpacerItem(new QSpacerItem(220, 20, QSizePolicy::Fixed,
                                                QSizePolicy::Fixed));
    buttonLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum,
                                                QSizePolicy::Expanding));
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(groupBox);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void RecordAdEdView::cancel()
{
    closeConnection();
}

bool RecordAdEdView::isError()
{
    if (titleLineEdit->text() == "")
    {
        showError(tr("You should enter title of the record"));
        titleLineEdit->setFocus();
        return true;
    }
    return false;
}

void RecordAdEdView::showError(const QString &error)
{
    errorTitleLabel->setText(QString("<font color=red>").append(error).append("</font>"));
    errorTitleLabel->setVisible(true);
}

void RecordAdEdView::ok()
{
    if (isError()) return;
    MegaTcpSocket *tcpSocket = MegaTcpSocket::Instance();
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(sendRequest()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(getResponse()));
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

void RecordAdEdView::sendRequest()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << quint16(0);
    switch (mode)
    {
        case AddMode:
            out << MegaProtocol::ADD_RECORD << this->table;
            break;
        case EditMode:
            out << MegaProtocol::EDIT_RECORD << this->table << this->oldTitle;
            break;
        default:
            break;
    }
    QVariant::Type type = (QVariant::Type) typeComboBox->itemData(
                typeComboBox->currentIndex(),Qt::UserRole).toUInt();
    QString valueText = valueLineEdit->text();
    QVariant value = convert(valueText, type);

    out << titleLineEdit->text() << commentTextEdit->toPlainText()
        << readOnlyCheckBox->isChecked() << type << value;

    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    MegaTcpSocket::Instance()->write(block);
}

void RecordAdEdView::getResponse()
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
                success = tr("Record added successfully");
                break;
            case EditMode:
                success = tr("Record edited successfully");
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
        case MegaProtocol::RECORD_EXIST:
            showError(tr("Record with this title already exist"));
            titleLineEdit->selectAll();
            titleLineEdit->setFocus();
            tcpSocket->abort();
            okButton->setEnabled(true);
            nextBlockSize = 0;
            break;
        case MegaProtocol::RECORD_DELETED:
            showError(tr("This record is already deleted"));
            tcpSocket->abort();
            okButton->setVisible(false);
            cancelButton->setVisible(false);
            backToTableListButton->setVisible(false);
            backButton->setVisible(true);
            nextBlockSize = 0;
            break;
        case MegaProtocol::TABLE_DELETED:
            showError(tr("Table with this record is already deleted"));
            tcpSocket->abort();
            okButton->setVisible(false);
            cancelButton->setVisible(false);
            backButton->setVisible(false);
            backToTableListButton->setVisible(true);
            nextBlockSize = 0;
            break;
        default:
            break;
    }
}

void RecordAdEdView::error()
{
    MainWindow::Instance()->setStatusLabelText(
            MegaTcpSocket::Instance()->errorString());
    closeConnection();
}

void RecordAdEdView::connectionClosedByServer()
{
    if (nextBlockSize != 0xFFFF)
    {
        MainWindow::Instance()->setStatusLabelText(
                tr("Error: Connection closed by server"));
    }
}

void RecordAdEdView::closeConnection()
{
    okButton->setEnabled(true);
    MegaTcpSocket *tcpSocket = MegaTcpSocket::Instance();
    disconnect(tcpSocket, SIGNAL(connected()), this, SLOT(sendRequest()));
    disconnect(tcpSocket, SIGNAL(readyRead()), this, SLOT(getResponse()));
    disconnect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
               this, SLOT(error()));
    disconnect(tcpSocket, SIGNAL(disconnected()),
               this, SLOT(connectionClosedByServer()));
    tcpSocket->abort();
    RecordListWidget::Instance()->show(table);
}

void RecordAdEdView::show(const QString &table,
                          const RecordAdEdView::Mode &mode,
                          const QString &title,
                          const QString &comment,
                          const bool &readOnly,
                          const QVariant::Type &type,
                          const QVariant &value)
{
    this->oldTitle = title;
    this->table = table;
    this->readOnly = readOnly ? Qt::Checked : Qt::Unchecked;
    this->typeComboBoxIndex = typeComboBox->findData((uint) type);

    titleLineEdit->setText(title);
    commentTextEdit->setText(comment);
    readOnlyCheckBox->setChecked(readOnly);
    typeComboBox->setCurrentIndex(this->typeComboBoxIndex);
    setValidatorComboBoxCurrentIndexChanged(typeComboBox->currentIndex());
    valueLineEdit->setText(value.toString());
    this->mode = mode;
    switch (mode)
    {
        case ViewMode:
            viewMode();
            groupBox->setTitle(tr("View record"));
            break;
        case EditMode:
            groupBox->setTitle(tr("Edit record"));
            if (readOnly)
            {
                viewMode();
                break;
            }
        case AddMode:
            groupBox->setTitle(tr("Add record"));
            connect(typeComboBox, SIGNAL(currentIndexChanged(int)),
                    this, SLOT(setValidatorComboBoxCurrentIndexChanged(int)));
            okButton->setVisible(true);
            cancelButton->setVisible(true);
            backButton->setVisible(false);
            backToTableListButton->setVisible(false);
            break;
    }

    MainWindow::Instance()->setCentralWidget(this);
    MainWindow::Instance()->setStatusLabelText("");
}

void RecordAdEdView::viewMode()
{
    titleLineEdit->setReadOnly(true);
    commentTextEdit->setReadOnly(true);
    connect(readOnlyCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(readOnlyCheckBoxStateChanged()));
    connect(typeComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(readOnlyComboBoxCurrentIndexChanged()));
    valueLineEdit->setReadOnly(true);
    okButton->setVisible(false);
    cancelButton->setVisible(false);
    backToTableListButton->setVisible(false);
    backButton->setVisible(true);
}

void RecordAdEdView::readOnlyCheckBoxStateChanged()
{
    readOnlyCheckBox->setCheckState(this->readOnly);
}

void RecordAdEdView::readOnlyComboBoxCurrentIndexChanged()
{
    typeComboBox->setCurrentIndex(this->typeComboBoxIndex);
}

void RecordAdEdView::setValidatorComboBoxCurrentIndexChanged(const int &index)
{
    QVariant::Type type = (QVariant::Type) typeComboBox->itemData(
            index, Qt::UserRole).toUInt();
    switch (type)
    {
        case QVariant::Double:
            valueLineEdit->setValidator(new QDoubleValidator(valueLineEdit));
            valueLineEdit->setText("0.0");
            break;
        case QVariant::Int:
            valueLineEdit->setValidator(new QIntValidator(valueLineEdit));
            valueLineEdit->setText("0");
            break;
        case QVariant::String:
            valueLineEdit->setValidator(NULL);
            valueLineEdit->setText("");
            break;
        default:
            break;
    }
}

void RecordAdEdView::hideError()
{
    errorTitleLabel->setVisible(false);
}

void RecordAdEdView::backToTableList()
{
    TableListWidget::Instance()->show();
}
