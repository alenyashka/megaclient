#ifndef RECORDLISTWIDGET_H
#define RECORDLISTWIDGET_H

#include <QtGui>
#include <QWidget>
#include "megatcpsocket.h"
#include "megaprotocol.h"
#include "mainwindow.h"

class RecordListWidget : public QWidget
{
    Q_OBJECT
public:
    RecordListWidget(const QString&);
private slots:
    void updateRecordsList();
    void sendUpdateRecordsListRequest();
    void getUpdateRecordsListResponse();
    void errorUpdateRecordsList();
    void connectionUpdateRecordsListClosedByServer();
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
