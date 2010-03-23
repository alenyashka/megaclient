#ifndef RECORDADEDVIEW_H
#define RECORDADEDVIEW_H

#include <QtGui>
#include <QWidget>
#include "singleton.h"
#include "mainwindow.h"
#include "megatcpsocket.h"
#include "megaprotocol.h"
#include "util.h"

class RecordAdEdView : public QWidget, public Singleton<RecordAdEdView>
{
    Q_OBJECT
public:
    enum Mode
    {
        ViewMode, AddMode, EditMode, EditValueMode
    };
protected:
    RecordAdEdView();
    friend class Singleton<RecordAdEdView>;
public:
    void show(const QString &table,
              const RecordAdEdView::Mode &mode,
              const QString &title = "",
              const QString &comment = "",
              const bool &readOnly = false,
              const QVariant::Type &type = QVariant::Double,
              const QVariant &value = 0.0);
private slots:
    void ok();
    void cancel();
    void readOnlyCheckBoxStateChanged();
    void readOnlyComboBoxCurrentIndexChanged();
    void setValidatorComboBoxCurrentIndexChanged(const int&);
    void sendRequest();
    void getResponse();
    void error();
    void connectionClosedByServer();
    void hideError();
    void backToTableList();
private:
    bool isError();
    void closeConnection();
    void showError(const QString&);
    void viewMode();
    Mode mode;
    QString table;
    QString oldTitle;
    Qt::CheckState readOnly;
    int typeComboBoxIndex;
    QLabel *titleLabel;
    QLabel *commentLabel;
    QLabel *readOnlyLabel;
    QLabel *typeLabel;
    QLabel *valueLabel;
    QLabel *errorTitleLabel;
    QLineEdit *titleLineEdit;
    QTextEdit *commentTextEdit;
    QCheckBox *readOnlyCheckBox;
    QComboBox *typeComboBox;
    QLineEdit *valueLineEdit;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QPushButton *backButton;
    QPushButton *backToTableListButton;
    QGroupBox *groupBox;
    quint16 nextBlockSize;

};

#endif // RECORDADEDVIEW_H
