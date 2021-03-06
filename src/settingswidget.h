#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

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
    QLabel *timeoutLabel;
    QLabel *refreshLabel;
    QLabel *passLabel;
    QLineEdit *hostLineEdit;
    QLineEdit *portLineEdit;
    QLineEdit *timeoutLineEdit;
    QLineEdit *refreshLineEdit;
    QLineEdit *passLineEdit;
};

#endif // SETTINGSWIDGET_H
