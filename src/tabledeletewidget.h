#ifndef TABLEDELETEWIDGET_H
#define TABLEDELETEWIDGET_H

#include "tablelistwidget.h"

class TableDeleteWidget : public QWidget, public Singleton<TableDeleteWidget>
{
    Q_OBJECT
public:
    void show(const QString&);
protected:
    TableDeleteWidget();
    friend class Singleton<TableDeleteWidget>;
private slots:
    void yes();
    void cancel();
    void sendRequest();
    void getResponse(QByteArray);
private:
    void closeConnection();
    QString tableName;
    QPushButton *yesButton;
    QPushButton *noButton;
    QGroupBox *groupBox;
    QLabel *messageLabel;
    quint16 nextBlockSize;
};

#endif // TABLEDELETEWIDGET_H
