#ifndef TABLELISTWIDGET_H
#define TABLELISTWIDGET_H

#include "mainwindow.h"
#include "megaprotocol.h"
#include "tableadedwidget.h"
#include "tabledeletewidget.h"
#include "settingswidget.h"
#include "megaguardwidget.h"
#include "recordlistwidget.h"

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
