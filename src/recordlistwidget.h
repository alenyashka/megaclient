#ifndef RECORDLISTWIDGET_H
#define RECORDLISTWIDGET_H

#include "tablelistwidget.h"
#include "recordadedview.h"
#include "recorddeletewidget.h"

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
    void getUpdateRecordsListResponse(QByteArray);
    void backToTableList();
    void addRecord();
    void propRecord();
    void delRecord();
private:
    void showError(const QString&);
    void closeUpdateRecordsListConnection();
    QString tableName;
    QPushButton *backButton;
    QPushButton *addRecordButton;
    QPushButton *propRecordButton;
    QPushButton *delRecordButton;
    QTableWidget *recordTableWidget;
    QLabel *errorLabel;
    QGroupBox *groupBox;
    quint16 nextBlockSize;
};

#endif // RECORDLISTWIDGET_H
