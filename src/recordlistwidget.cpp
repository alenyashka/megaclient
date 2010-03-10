#include "recordlistwidget.h"

RecordListWidget::RecordListWidget()
{
    backButton = new QPushButton(tr("Back"));
    backButton->setStatusTip(tr("Back to the table's list"));
    backButton->setIcon(QIcon(":images/back.png"));
    connect(backButton, SIGNAL(clicked()),
            this, SLOT(backToTableList()));

    viewRecordButton = new QPushButton(tr("View record"));
    viewRecordButton->setStatusTip(tr("View current record"));
    viewRecordButton->setShortcut(tr("Ctrl+Alt+V"));
    viewRecordButton->setIcon(QIcon(":images/view.png"));
    connect(viewRecordButton, SIGNAL(clicked()), this, SLOT(viewRecord()));

    addRecordButton = new QPushButton(tr("Add record"));
    addRecordButton->setStatusTip(tr("Add record to the table"));
    addRecordButton->setShortcut(tr("Ctrl+Alt+A"));
    addRecordButton->setIcon(QIcon(":images/add.png"));
    connect(addRecordButton, SIGNAL(clicked()), this, SLOT(addRecord()));

    editRecordButton = new QPushButton(tr("Edit record"));
    editRecordButton->setStatusTip(tr("Edit current record"));
    editRecordButton->setShortcut(tr("Ctrl+Alt+E"));
    editRecordButton->setIcon(QIcon(":/images/edit.png"));
    connect(editRecordButton, SIGNAL(clicked()), this, SLOT(editRecord()));

    delRecordButton = new QPushButton(tr("Delete record"));
    delRecordButton->setStatusTip(tr("Delete current record from the list"));
    delRecordButton->setShortcut(tr("Ctrl+Alt+D"));
    delRecordButton->setIcon(QIcon(":/images/del.png"));
    connect(delRecordButton, SIGNAL(clicked()), this, SLOT(delRecord()));

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(viewRecordButton);
    rightLayout->addWidget(addRecordButton);
    rightLayout->addWidget(editRecordButton);
    rightLayout->addWidget(delRecordButton);
    rightLayout->addWidget(backButton);
    rightLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum,
            QSizePolicy::Expanding));

    titleLabel = new QLabel();
    titleLabel->setFont(QFont("AlArabia", 20, 50, false));

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

void RecordListWidget::show(const QString &table)
{
    this->tableName = table;
    MainWindow::Instance()->setCentralWidget(this);
    MainWindow::Instance()->setStatusLabelText("");
    titleLabel->setText(tr("Table: <i>%1</i>. Record list").arg(tableName));
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
    tcpSocket->abort();
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
    tcpSocket->abort();
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
        bool readOnly;
        QVariant::Type type;
        QVariant value;
        in >> title >> comment >> readOnly >> type >> value;
        recordTableWidget->setRowCount(row + 1);

        recordTableWidget->setItem(row, 0, new QTableWidgetItem(title));
        recordTableWidget->setItem(row, 1, new QTableWidgetItem(comment));
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignCenter);
        item->data(Qt::CheckStateRole);
        item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        if (readOnly)
        {
            item->setCheckState(Qt::Checked);
        }
        else
        {
            item->setCheckState(Qt::Unchecked);
        }
        recordTableWidget->setItem(row, 2, item);
        item = new QTableWidgetItem();
        QString t = type == QVariant::Double ? "Double" :
                    type == QVariant::Int ? "Int" : "string";
        item->setText(t);
        item->setData(Qt::UserRole, type);
        recordTableWidget->setItem(row, 3, item);
        item = new QTableWidgetItem();
        item->setText(value.toString());
        item->setData(Qt::UserRole, value);
        recordTableWidget->setItem(row, 4, item);
        nextBlockSize = 0;
    }
}

void RecordListWidget::backToTableList()
{
    TableListWidget::Instance()->show();
}

void RecordListWidget::viewRecord()
{
    int row = recordTableWidget->currentRow();
    if (!(row < 0))
    {
        QString title = recordTableWidget->item(row, 0)->text();
        QString comment = recordTableWidget->item(row, 1)->text();
        bool readOnly = recordTableWidget->item(row, 2)->checkState();
        QVariant::Type type = recordTableWidget->item(row ,3)->data(
                Qt::UserRole).type();
        QVariant value = recordTableWidget->item(row, 4)->data(Qt::UserRole);
        RecordAdEdView::Instance()->show(tableName, RecordAdEdView::ViewMode,
                                         title, comment, readOnly, type, value);
    }
}

void RecordListWidget::addRecord()
{
    RecordAdEdView::Instance()->show(tableName, RecordAdEdView::AddMode);
}

void RecordListWidget::editRecord()
{
    int row = recordTableWidget->currentRow();
    if (!(row < 0))
    {
        QString title = recordTableWidget->item(row, 0)->text();
        QString comment = recordTableWidget->item(row, 1)->text();
        bool readOnly = recordTableWidget->item(row, 2)->checkState();
        QVariant::Type type = recordTableWidget->item(row ,3)->data(
                Qt::UserRole).type();
        QVariant value = recordTableWidget->item(row, 4)->data(Qt::UserRole);
        RecordAdEdView::Instance()->show(tableName, RecordAdEdView::EditMode,
                                         title, comment, readOnly, type, value);
    }
}

void RecordListWidget::delRecord()
{
    int row = recordTableWidget->currentRow();
    if (!(row < 0))
    {
        QString title = recordTableWidget->item(row, 0)->text();
        RecordDeleteWidget::Instance()->show(this->tableName, title);
    }
}
