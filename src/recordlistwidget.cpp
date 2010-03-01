#include "recordlistwidget.h"

RecordListWidget::RecordListWidget(const QString &table)
{
    this->tableName = table;
    backButton = new QPushButton(tr("Back"));
    backButton->setStatusTip(tr("Back to the table's list"));
    backButton->setIcon(QIcon(":images/back.png"));

    viewRecordButton = new QPushButton(tr("View record"));
    viewRecordButton->setStatusTip(tr("View current record"));
    viewRecordButton->setShortcut(tr("Ctrl+Alt+V"));
    viewRecordButton->setIcon(QIcon(":images/view.png"));

    addRecordButton = new QPushButton(tr("Add record"));
    addRecordButton->setStatusTip(tr("Add record to the table"));
    addRecordButton->setShortcut(tr("Ctrl+Alt+A"));
    addRecordButton->setIcon(QIcon(":images/add.png"));

    editRecordButton = new QPushButton(tr("Edit record"));
    editRecordButton->setStatusTip(tr("Edit current record"));
    editRecordButton->setShortcut(tr("Ctrl+Alt+E"));
    editRecordButton->setIcon(QIcon(":/images/edit.png"));

    delRecordButton = new QPushButton(tr("Delete record"));
    delRecordButton->setStatusTip(tr("Delete current record from the list"));
    delRecordButton->setShortcut(tr("Ctrl+Alt+D"));
    delRecordButton->setIcon(QIcon(":/images/del.png"));

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(viewRecordButton);
    rightLayout->addWidget(addRecordButton);
    rightLayout->addWidget(editRecordButton);
    rightLayout->addWidget(delRecordButton);
    rightLayout->addWidget(backButton);
    rightLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum,
            QSizePolicy::Expanding));

    titleLabel = new QLabel(tr("Table's list"));
    titleLabel->setFont(QFont("Arial", 18, 10, false));

    recordTableWidget = new QTableWidget();
    recordTableWidget->setColumnCount(5);
    recordTableWidget->verticalHeader()->hide();
    recordTableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(
            tr("Title")));
    recordTableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(tr(
            "Comment")));
    recordTableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem(tr(
                "ReadOnly")));
    recordTableWidget->setHorizontalHeaderItem(3, new QTableWidgetItem(tr(
                "Type")));
    recordTableWidget->setHorizontalHeaderItem(4, new QTableWidgetItem(tr(
                "Value")));
    recordTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    recordTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    recordTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    QHBoxLayout *centralLayout = new QHBoxLayout;
    centralLayout->addWidget(recordTableWidget);
    centralLayout->addLayout(rightLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(centralLayout);
    setLayout(mainLayout);
    updateRecordsList();
}

void RecordListWidget::updateRecordsList()
{
    MegaTcpSocket *tcpSocket = MegaTcpSocket::Instance();
    connect(tcpSocket, SIGNAL(connected()),
            this, SLOT(sendUpdateRecordsListRequest()));
    connect(tcpSocket, SIGNAL(readyRead()),
            this, SLOT(getUpdateRecordsListResponse()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(errorUpdateRecordsList()));
    connect(tcpSocket, SIGNAL(disconnected()),
            this, SLOT(connectionUpdateRecordsListClosedByServer()));
    viewRecordButton->setEnabled(false);
    addRecordButton->setEnabled(false);
    editRecordButton->setEnabled(false);
    delRecordButton->setEnabled(false);
    tcpSocket->connectToHost();
    recordTableWidget->setRowCount(0);
    MainWindow::Instance()->setStatusLabelText(tr("Connecting to server..."));
    nextBlockSize = 0;
}

void RecordListWidget::connectionUpdateRecordsListClosedByServer()
{
    if (nextBlockSize != 0xFFFF)
    {
        MainWindow::Instance()->setStatusLabelText(
                tr("Error: Connection closed by server"));
    }
    closeUpdateRecordsListConnection();
}

void RecordListWidget::closeUpdateRecordsListConnection()
{
    MegaTcpSocket *tcpSocket = MegaTcpSocket::Instance();
    disconnect(tcpSocket, SIGNAL(connected()),
               this, SLOT(sendUpdateRecordsListRequest()));
    disconnect(tcpSocket, SIGNAL(readyRead()),
               this, SLOT(getUpdateRecordsListResponse()));
    disconnect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
               this, SLOT(errorUpdateRecordsList()));
    disconnect(tcpSocket, SIGNAL(disconnected()),
               this, SLOT(connectionUpdateRecordsListClosedByServer()));
    if (recordTableWidget->rowCount() > 0)
    {
        recordTableWidget->selectRow(0);
        viewRecordButton->setEnabled(true);
        addRecordButton->setEnabled(true);
        editRecordButton->setEnabled(true);
        delRecordButton->setEnabled(true);
    }
}

void RecordListWidget::errorUpdateRecordsList()
{
    MainWindow::Instance()->setStatusLabelText(
            MegaTcpSocket::Instance()->errorString());
    closeUpdateRecordsListConnection();
}

void RecordListWidget::sendUpdateRecordsListRequest()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << quint16(0) << MegaProtocol::GET_RECORDS_LIST << tableName;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    out.device()->seek(0);
    MegaTcpSocket::Instance()->write(block);
    out << quint16(0xFFFF);
    MegaTcpSocket::Instance()->write(block);
}

void RecordListWidget::getUpdateRecordsListResponse()
{
    MegaTcpSocket *tcpSocket = MegaTcpSocket::Instance();
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_5);
    forever
    {
        int row = recordTableWidget->rowCount();
        if (nextBlockSize == 0)
        {
            if (tcpSocket->bytesAvailable() < sizeof(quint16)) break;
            in >> nextBlockSize;
        }
        if (nextBlockSize == 0xFFFF)
        {
            closeUpdateRecordsListConnection();
            MainWindow::Instance()->setStatusLabelText(
                    tr("Receipt %1 record(s)").arg(row));
            break;
        }
        if (tcpSocket->bytesAvailable() < nextBlockSize) break;
        QString title;
        QString comment;
        QString readOnly;
        QString type;
        QString value;
        in >> title >> comment >> readOnly >> type >> value;
        QStringList fields;
        fields << title << comment << readOnly << type << value;
        recordTableWidget->setRowCount(row + 1);
        for (int i = 0; i < fields.count(); ++i)
        {
            recordTableWidget->setItem(row, i, new QTableWidgetItem(fields[i]));
        }
        nextBlockSize = 0;
    }
}
