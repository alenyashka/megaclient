#ifndef RECORDADEDVIEW_H
#define RECORDADEDVIEW_H

#include <QtGui>
#include <QWidget>
#include "singleton.h"
#include "mainwindow.h"

class RecordAdEdView : public QWidget, public Singleton<RecordAdEdView>
{
    Q_OBJECT
public:
    enum Mode
    {
        ViewMode, AddMode, EditMode
    };
protected:
    RecordAdEdView();
    friend class Singleton<RecordAdEdView>;
public slots:
    void show(const QString&, const QString&, const bool&,
              const QVariant::Type&, const QVariant&, const QString&,
              const Mode&);
private slots:
    void ok();
    void cancel();
    void readOnlyCheckBoxStateChanged();
    void readOnlyComboBoxCurrentIndexChanged();
private:
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
    QLineEdit *titleLineEdit;
    QTextEdit *commentTextEdit;
    QCheckBox *readOnlyCheckBox;
    QComboBox *typeComboBox;
    QLineEdit *valueLineEdit;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QPushButton *backButton;
    QGroupBox *groupBox;

};

#endif // RECORDADEDVIEW_H
