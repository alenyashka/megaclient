#ifndef RECORDLISTWIDGET_H
#define RECORDLISTWIDGET_H

#include <QtGui>
#include <QWidget>
#include "megatcpsocket.h"
#include "megaprotocol.h"
#include "mainwindow.h"
#include "singleton.h"
#include "recordadedview.h"

class RecordListWidget : public QWidget, public Singleton<RecordListWidget>
{
    Q_OBJECT
public:
    void show(const QString&);
protected:
    RecordListWidget();
    friend class Singleton<RecordListWidget>;
private slots:
    void updateRecordsList();
    void sendUpdateRecordsListRequest();
    void getUpdateRecordsListResponse();
    void errorUpdateRecordsList();
    void connectionUpdateRecordsListClosedByServer();
    void backToTableList();
    void viewRecord();
    void addRecord();
    void editRecord();
private:
    void closeUpdateRecordsListConnection();
    QString tableName;
    QPushButton *backButton;
    QPushButton *viewRecordButton;
    QPushButton *addRecordButton;
    QPushButton *editRecordButton;
    QPushButton *delRecordButton;
    QTableWidget *recordTableWidget;
    QLabel *titleLabel;
    quint16 nextBlockSize;
};

#endif // RECORDLISTWIDGET_H
