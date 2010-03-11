#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QtGui>
#include <QWidget>
#include "singleton.h"
#include "megatcpsocket.h"
#include "tablelistwidget.h"

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
    QLineEdit *hostLineEdit;
    QLineEdit *portLineEdit;
};

#endif // SETTINGSWIDGET_H
