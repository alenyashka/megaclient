#include "recordlistwidget.h"

RecordListWidget::RecordListWidget()
{
    backButton = new QPushButton(tr("Back"));
    backButton->setStatusTip(tr("Back to the table's list"));
    backButton->setIcon(QIcon(":images/back.png"));
    connect(backButton, SIGNAL(clicked()),
            this, SLOT(backToTableList()));

    addRecordButton = new QPushButton(tr("Add record"));
    addRecordButton->setStatusTip(tr("Add record to the table"));
    addRecordButton->setShortcut(tr("Ctrl+Alt+A"));
    addRecordButton->setIcon(QIcon(":images/add.png"));
    connect(addRecordButton, SIGNAL(clicked()), this, SLOT(addRecord()));

    propRecordButton = new QPushButton(tr("Record's properties"));
    propRecordButton->setStatusTip(tr("View or edit current record"));
    propRecordButton->setShortcut(tr("Ctrl+Alt+P"));
    propRecordButton->setIcon(QIcon(":/images/edit.png"));
    connect(propRecordButton, SIGNAL(clicked()), this, SLOT(propRecord()));

    delRecordButton = new QPushButton(tr("Delete record"));
    delRecordButton->setStatusTip(tr("Delete current record from the list"));
    delRecordButton->setShortcut(tr("Ctrl+Alt+D"));
    delRecordButton->setIcon(QIcon(":/images/del.png"));
    connect(delRecordButton, SIGNAL(clicked()), this, SLOT(delRecord()));

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(addRecordButton);
    rightLayout->addWidget(propRecordButton);
    rightLayout->addWidget(delRecordButton);
    rightLayout->addWidget(backButton);
    rightLayout->addSpacerItem(new QSpacerItem(220, 20, QSizePolicy::Fixed,
                                               QSizePolicy::Fixed));
    rightLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum,
            QSizePolicy::Expanding));

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
    connect(recordTableWidget, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(propRecord()));

    errorLabel = new QLabel();
    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addWidget(recordTableWidget);
    centralLayout->addWidget(errorLabel);

    groupBox = new QGroupBox();
    groupBox->setLayout(centralLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(groupBox);
    mainLayout->addLayout(rightLayout);
    setLayout(mainLayout);
}

void RecordListWidget::show(const QString &table)
{
    this->tableName = table;
    MainWindow::Instance()->setCentralWidget(this);
    MainWindow::Instance()->setStatusLabelText("");
    groupBox->setTitle(tr("Table: [%1]").arg(tableName));
    errorLabel->setVisible(false);
    switch (MegaGuard::Instance()->getMode())
    {
        case User:
            delRecordButton->setVisible(false);
            addRecordButton->setVisible(false);
            break;
        case Admin:
            delRecordButton->setVisible(true);
            addRecordButton->setVisible(true);
            break;
    }
    connect(MegaConnector::Instance(), SIGNAL(refresh()),
            this, SLOT(updateRecordsList()));
    connect(MegaConnector::Instance(), SIGNAL(response(QByteArray)),
            this, SLOT(getUpdateRecordsListResponse(QByteArray)));
    updateRecordsList();
}

void RecordListWidget::updateRecordsList()
{
    addRecordButton->setEnabled(false);
    propRecordButton->setEnabled(false);
    delRecordButton->setEnabled(false);
    recordTableWidget->setRowCount(0);
    nextBlockSize = 0;
    sendUpdateRecordsListRequest();
}

void RecordListWidget::closeUpdateRecordsListConnection()
{
    addRecordButton->setEnabled(true);
    if (recordTableWidget->rowCount() > 0)
    {
        recordTableWidget->selectRow(0);
        propRecordButton->setEnabled(true);
        delRecordButton->setEnabled(true);
    }
}

void RecordListWidget::sendUpdateRecordsListRequest()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << quint16(0) << MegaProtocol::GET_RECORDS_LIST << tableName;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    MegaConnector::Instance()->write(block);
    block.clear();
    out.device()->seek(0);
    out << quint16(0xFFFF);
    MegaConnector::Instance()->write(block);
}

void RecordListWidget::getUpdateRecordsListResponse(QByteArray block)
{
    QDataStream in(block);
    in.setVersion(QDataStream::Qt_4_5);
    uint state;
    in >> state;
    if (state == MegaProtocol::OK)
    {
        forever
        {
            int row = recordTableWidget->rowCount();
            if (nextBlockSize == 0)
            {
                if (in.device()->bytesAvailable() < sizeof(quint16)) break;
                in >> nextBlockSize;
            }
            if (nextBlockSize == 0xFFFF)
            {
                closeUpdateRecordsListConnection();
                MainWindow::Instance()->setStatusLabelText(
                        tr("Receipt %1 record(s)").arg(row));
                break;
            }
            if (in.device()->bytesAvailable() < nextBlockSize) break;
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
                        type == QVariant::Int ? "Int" : "String";
            item->setText(t);
            item->setData(Qt::UserRole, type);
            recordTableWidget->setItem(row, 3, item);
            item = new QTableWidgetItem();
            item->setText(value.toString());
            item->setData(Qt::UserRole, value);
            recordTableWidget->setItem(row, 4, item);
            nextBlockSize = 0;
        }
        recordTableWidget->resizeColumnsToContents();
        recordTableWidget->resizeRowsToContents();
    }
    else
    {
        recordTableWidget->setVisible(false);
        addRecordButton->setVisible(false);
        propRecordButton->setVisible(false);
        delRecordButton->setVisible(false);
        uint err;
        in >> err;
        switch (err)
        {
            case MegaProtocol::TABLE_DELETED:
                showError(tr("This table is already deleted"));
                break;
            default:
                break;
        }
    }
}

void RecordListWidget::backToTableList()
{
    TableListWidget::Instance()->show();
}

void RecordListWidget::addRecord()
{
    RecordAdEdView::Instance()->show(tableName, RecordAdEdView::AddMode);
}

void RecordListWidget::propRecord()
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
        RecordAdEdView::Mode mode = MegaGuard::Instance()->getMode() == Admin
                                    ? RecordAdEdView::EditMode
                                    : RecordAdEdView::EditValueMode;
        RecordAdEdView::Instance()->show(tableName, mode,
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

void RecordListWidget::showError(const QString &error)
{
    errorLabel->setText(QString("<font color=red>").append(error)
                        .append("</font>"));
    errorLabel->setVisible(true);
}
