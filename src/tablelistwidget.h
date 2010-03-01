#ifndef TABLELISTWIDGET_H
#define TABLELISTWIDGET_H

#include <QtGui>
#include <QWidget>
#include "megatcpsocket.h"
#include "megaprotocol.h"
#include "mainwindow.h"

class TableListWidget : public QWidget
{
    Q_OBJECT
public:
    TableListWidget();
private slots:
    void updateTablesList();
    void sendUpdateTablesListRequest();
    void getUpdateTablesListResponse();
    void errorUpdateTablesList();
    void connectionUpdateTablesListClosedByServer();
    void viewRecords();
private:
    void closeUpdateTablesListConnection();
    QPushButton *updateButton;
    QPushButton *settingsButton;
    QPushButton *quitButton;
    QPushButton *viewTableButton;
    QPushButton *addTableButton;
    QPushButton *editTableButton;
    QPushButton *delTableButton;
    QTableWidget *tableTableWidget;
    QLabel *titleLabel;
    quint16 nextBlockSize;
};

#endif // TABLELISTWIDGET_H
