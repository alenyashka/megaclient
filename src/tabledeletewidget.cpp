#include "tabledeletewidget.h"

TableDeleteWidget::TableDeleteWidget()
{
    yesButton = new QPushButton(tr("Yes"));
    yesButton->setStatusTip(tr("Finish the operation"));
    yesButton->setIcon(QIcon(":/images/ok.png"));
    connect(yesButton, SIGNAL(clicked()), this, SLOT(yes()));

    noButton = new QPushButton(tr("No"));
    noButton->setToolTip(tr("Cancel the operation"));
    noButton->setIcon(QIcon(":/images/cancel.png"));
    connect(noButton, SIGNAL(clicked()), this, SLOT(cancel()));

    messageLabel = new QLabel();

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(yesButton);
    buttonLayout->addWidget(noButton);
    buttonLayout->addSpacerItem(new QSpacerItem(220, 20, QSizePolicy::Fixed,
                                                QSizePolicy::Fixed));
    buttonLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum,
                                                QSizePolicy::Expanding));

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(messageLabel);
    leftLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum,
                                                QSizePolicy::Expanding));
    groupBox = new QGroupBox(tr("Delete table"));
    groupBox->setLayout(leftLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(groupBox);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void TableDeleteWidget::show(const QString &name)
{
    this->tableName = name;
    MainWindow::Instance()->setCentralWidget(this);
    MainWindow::Instance()->setStatusLabelText("");
    messageLabel->setText(tr("Now table %1 will be delete.\n"
                             "This action can not be reverted.\n"
                             "Do you agree?").arg(name));
    connect(MegaConnector::Instance(), SIGNAL(response(QByteArray)),
            this, SLOT(getResponse(QByteArray)));
}

void TableDeleteWidget::cancel()
{
    TableListWidget::Instance()->show();
}

void TableDeleteWidget::yes()
{
    yesButton->setEnabled(false);
    noButton->setEnabled(false);
    nextBlockSize = 0;
    sendRequest();
}

void TableDeleteWidget::closeConnection()
{
    yesButton->setEnabled(true);
    noButton->setEnabled(true);
    TableListWidget::Instance()->show();
}

void TableDeleteWidget::sendRequest()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << quint16(0);
    out << MegaProtocol::DEL_TABLE << this->tableName;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    MegaConnector::Instance()->write(block);
    block.clear();
    out.device()->seek(0);
    out <<  quint16(0xFFFF);
    MegaConnector::Instance()->write(block);
}

void TableDeleteWidget::getResponse(QByteArray block)
{
    QDataStream in(block);
    in.setVersion(QDataStream::Qt_4_5);

    if (nextBlockSize == 0)
    {
        if (in.device()->bytesAvailable() < sizeof(quint16)) return;
        in >> nextBlockSize;
    }
    if (nextBlockSize == 0xFFFF)
    {
        closeConnection();
        MainWindow::Instance()->setStatusLabelText(
                tr("Table deleted successfully"));
        return;
    }
    nextBlockSize = 0;
}
