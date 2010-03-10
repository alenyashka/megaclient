#ifndef RECORDDELETEWIDGET_H
#define RECORDDELETEWIDGET_H

#include <QtGui>
#include <QWidget>
#include "singleton.h"
#include "recordlistwidget.h"
#include "megatcpsocket.h"

class RecordDeleteWidget : public QWidget, public Singleton<RecordDeleteWidget>
{
    Q_OBJECT
public:
    void show(const QString&, const QString&);
protected:
    RecordDeleteWidget();
    friend class Singleton<RecordDeleteWidget>;
private slots:
    void yes();
    void cancel();
    void sendRequest();
    void getResponse();
    void error();
    void connectionClosedByServer();
private:
    void showError(const QString&);
    void closeConnection();
    QString tableName;
    QString recordTitle;
    QGroupBox *groupBox;
    QPushButton *yesButton;
    QPushButton *noButton;
    QLabel *titleLabel;
    QLabel *messageLabel;
    QLabel *errorLabel;
    quint16 nextBlockSize;
};

#endif // RECORDDELETEWIDGET_H