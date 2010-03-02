#include "tablelistwidget.h"

TableListWidget::TableListWidget()
{
    updateButton = new QPushButton(tr("Update"));
    updateButton->setShortcut(tr("Ctrl+R"));
    updateButton->setStatusTip(tr("Update list of tables"));
    updateButton->setIcon(QIcon(":/images/update.png"));
    connect(updateButton, SIGNAL(clicked()), this, SLOT(updateTablesList()));

    settingsButton = new QPushButton(tr("Settings"));
    settingsButton->setStatusTip(tr("Edit settings of application"));
    settingsButton->setIcon(QIcon(":images/settings.png"));

    quitButton = new QPushButton(tr("Quit"));
    quitButton->setShortcut(tr("Ctrl+Q"));
    quitButton->setStatusTip(tr("Quit from application"));
    quitButton->setIcon(QIcon(":images/quit.png"));
    connect(quitButton, SIGNAL(clicked()),
            MainWindow::Instance(), SLOT(close()));

    viewTableButton = new QPushButton(tr("View table"));
    viewTableButton->setStatusTip(tr("View records of selected tables"));
    viewTableButton->setShortcut(tr("Ctrl+Alt+V"));
    viewTableButton->setIcon(QIcon(":images/view.png"));
    connect(viewTableButton, SIGNAL(clicked()), this, SLOT(viewRecords()));

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

    tableTableWidget = new QTableWidget();
    tableTableWidget->setColumnCount(2);
    tableTableWidget->verticalHeader()->hide();
    tableTableWidget->setHorizontalHeaderItem(0,
                                        new QTableWidgetItem(tr("Name")));
    tableTableWidget->setHorizontalHeaderItem(1,
                                        new QTableWidgetItem(tr("Comment")));
    tableTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    QHBoxLayout *centralLayout = new QHBoxLayout;
    centralLayout->addWidget(tableTableWidget);
    centralLayout->addLayout(rightLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(centralLayout);
    setLayout(mainLayout);
}

void TableListWidget::updateTablesList()
{
    MegaTcpSocket *tcpSocket = MegaTcpSocket::Instance();
    connect(tcpSocket, SIGNAL(connected()),
            this, SLOT(sendUpdateTablesListRequest()));
    connect(tcpSocket, SIGNAL(readyRead()),
            this, SLOT(getUpdateTablesListResponse()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(errorUpdateTablesList()));
    connect(tcpSocket, SIGNAL(disconnected()),
            this, SLOT(connectionUpdateTablesListClosedByServer()));
    tcpSocket->connectToHost();
    tableTableWidget->setRowCount(0);
    updateButton->setEnabled(false);
    viewTableButton->setEnabled(false);
    addTableButton->setEnabled(false);
    editTableButton->setEnabled(false);
    delTableButton->setEnabled(false);
    MainWindow::Instance()->setStatusLabelText(tr("Connecting to server..."));
    nextBlockSize = 0;
}

void TableListWidget::connectionUpdateTablesListClosedByServer()
{
    if (nextBlockSize != 0xFFFF)
    {
        MainWindow::Instance()->setStatusLabelText(
                tr("Error: Connection closed by server"));
    }
    closeUpdateTablesListConnection();
}

void TableListWidget::closeUpdateTablesListConnection()
{
    MegaTcpSocket *tcpSocket = MegaTcpSocket::Instance();
    disconnect(tcpSocket, SIGNAL(connected()),
               this, SLOT(sendUpdateTablesListRequest()));
    disconnect(tcpSocket, SIGNAL(readyRead()),
               this, SLOT(getUpdateTablesListResponse()));
    disconnect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
               this, SLOT(errorUpdateTablesList()));
    disconnect(tcpSocket, SIGNAL(disconnected()),
               this, SLOT(connectionUpdateTablesListClosedByServer()));
    updateButton->setEnabled(true);
    if (tableTableWidget->rowCount() > 0)
    {
        tableTableWidget->selectRow(0);
        viewTableButton->setEnabled(true);
        addTableButton->setEnabled(true);
        editTableButton->setEnabled(true);
        delTableButton->setEnabled(true);
    }
}

void TableListWidget::errorUpdateTablesList()
{
    MainWindow::Instance()->setStatusLabelText(
            MegaTcpSocket::Instance()->errorString());
    closeUpdateTablesListConnection();
}

void TableListWidget::sendUpdateTablesListRequest()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << quint16(0) << MegaProtocol::GET_TABLES_LIST;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    out.device()->seek(0);
    MegaTcpSocket::Instance()->write(block);
    out << quint16(0xFFFF);
    MegaTcpSocket::Instance()->write(block);
}

void TableListWidget::getUpdateTablesListResponse()
{
    MegaTcpSocket *tcpSocket = MegaTcpSocket::Instance();
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_5);
    forever
    {
        int row = tableTableWidget->rowCount();
        if (nextBlockSize == 0)
        {
            if (tcpSocket->bytesAvailable() < sizeof(quint16)) break;
            in >> nextBlockSize;
        }
        if (nextBlockSize == 0xFFFF)
        {
            closeUpdateTablesListConnection();
            MainWindow::Instance()->setStatusLabelText(
                    tr("Receipt %1 table(s)").arg(row));
            break;
        }
        if (tcpSocket->bytesAvailable() < nextBlockSize) break;
        QString name;
        QString comment;
        in >> name >> comment;
        tableTableWidget->setRowCount(row + 1);
        tableTableWidget->setItem(row, 0, new QTableWidgetItem(name));
        tableTableWidget->setItem(row, 1, new QTableWidgetItem(comment));
        nextBlockSize = 0;
    }
}

void TableListWidget::viewRecords()
{
    int row = tableTableWidget->currentRow();
    if (!(row < 0))
    {
        QString tableName = tableTableWidget->item(row, 0)->text();
        RecordListWidget::Instance()->show(tableName);
    }
}

void TableListWidget::show()
{
    MainWindow::Instance()->setCentralWidget(TableListWidget::Instance());
    updateTablesList();
}
