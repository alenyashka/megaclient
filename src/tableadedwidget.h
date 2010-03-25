#ifndef TABLEADEDWIDGET_H
#define TABLEADEDWIDGET_H

#include <QtGui>
#include <QWidget>
#include "singleton.h"
#include "mainwindow.h"
#include "megaconnector.h"
#include "megaprotocol.h"

class TableAdEdWidget : public QWidget, public Singleton<TableAdEdWidget>
{
    Q_OBJECT
protected:
    TableAdEdWidget();
    friend class Singleton<TableAdEdWidget>;
public:
    enum Mode
    {
        ViewMode, AddMode, EditMode
    };
    void show(const Mode&, const QString &name = "",
              const QString &comment = "");
private slots:
    void ok();
    void cancel();
    void sendRequest();
    void getResponse(QByteArray);
    void hideError();
private:
    bool isError();
    void showError(const QString&);
    void closeConnection();
    void readOnlyMode(const bool&);
    QGroupBox *groupBox;
    QLabel *nameLabel;
    QLabel *commentLabel;
    QLabel *errorLabel;
    QLineEdit *nameLineEdit;
    QTextEdit *commentTextEdit;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QPushButton *backButton;
    quint16 nextBlockSize;
    Mode mode;
    QString oldName;
};

#endif // TABLEADEDWIDGET_H
