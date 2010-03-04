#ifndef TABLEADEDWIDGET_H
#define TABLEADEDWIDGET_H

#include <QtGui>
#include <QWidget>
#include "singleton.h"
#include "mainwindow.h"
#include "megatcpsocket.h"
#include "megaprotocol.h"

class TableAdEdWidget : public QWidget, public Singleton<TableAdEdWidget>
{
    Q_OBJECT
protected:
    TableAdEdWidget();
    friend class Singleton<TableAdEdWidget>;
public:
    void show(const QString &name = "", const QString &comment = "");
private slots:
    void ok();
    void cancel();
    void sendRequest();
    void getResponse();
    void error();
    void connectionClosedByServer();
    void hideError();
private:
    enum Type {ADD = 0, EDIT = 1};
    bool isError();
    void showError(const QString&);
    void closeConnection();
    QLabel *titleLabel;
    QLabel *nameLabel;
    QLabel *commentLabel;
    QLabel *errorLabel;
    QLineEdit *nameLineEdit;
    QTextEdit *commentTextEdit;
    QPushButton *okButton;
    QPushButton *cancelButton;
    quint16 nextBlockSize;
    Type action;
    QString oldName;
};

#endif // TABLEADEDWIDGET_H
