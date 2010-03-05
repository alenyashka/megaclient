#ifndef TABLEDELETEWIDGET_H
#define TABLEDELETEWIDGET_H

#include <QtGui>
#include <QWidget>
#include "singleton.h"
#include "mainwindow.h"
#include "tablelistwidget.h"
class MainWindow;
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
    void getResponse();
    void error();
    void connectionClosedByServer();
private:
    void showError(const QString&);
    void closeConnection();
    QString tableName;
    QPushButton *yesButton;
    QPushButton *noButton;
    QLabel *titleLabel;
    QLabel *messageLabel;
    QLabel *errorLabel;
    quint16 nextBlockSize;
};

#endif // TABLEDELETEWIDGET_H
