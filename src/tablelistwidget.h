#ifndef TABLELISTWIDGET_H
#define TABLELISTWIDGET_H

#include <QtGui>
#include <QWidget>
#include "megaconnector.h"
#include "megaprotocol.h"
#include "mainwindow.h"
#include "singleton.h"
#include "tableadedwidget.h"
#include "tabledeletewidget.h"
#include "settingswidget.h"
#include "megaguardwidget.h"

class TableListWidget : public QWidget, public Singleton<TableListWidget>
{
    Q_OBJECT
protected:
    TableListWidget();
    friend class Singleton<TableListWidget>;
public:
    void show();
private slots:
    void updateTablesList();
    void sendUpdateTablesListRequest();
    void getUpdateTablesListResponse(QByteArray);
    void errorUpdateTablesList();
    void connectionUpdateTablesListClosedByServer();
    void viewRecords();
    void addTable();
    void propTable();
    void delTable();
    void settings();
    void changeMode();
private:
    void closeUpdateTablesListConnection();
    QPushButton *settingsButton;
    QPushButton *quitButton;
    QPushButton *viewRecordsButton;
    QPushButton *addTableButton;
    QPushButton *propTableButton;
    QPushButton *delTableButton;
    QPushButton *changeModeButton;
    QTableWidget *tableTableWidget;
    QGroupBox *groupBox;
    quint16 nextBlockSize;
};

#endif // TABLELISTWIDGET_H
