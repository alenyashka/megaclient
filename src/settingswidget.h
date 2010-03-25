#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QtGui>
#include <QWidget>
#include "singleton.h"
#include "tablelistwidget.h"
#include "megaguard.h"

class SettingsWidget : public QWidget, public Singleton<SettingsWidget>
{
    Q_OBJECT
public:
    void show();
protected:
    SettingsWidget();
    friend class Singleton<SettingsWidget>;
private slots:
    void ok();
    void cancel();
private:
    QPushButton *okButton;
    QPushButton *cancelButton;
    QGroupBox *groupBox;
    QLabel *hostLabel;
    QLabel *portLabel;
    QLabel *passLabel;
    QLineEdit *hostLineEdit;
    QLineEdit *portLineEdit;
    QLineEdit *passLineEdit;
};

#endif // SETTINGSWIDGET_H
