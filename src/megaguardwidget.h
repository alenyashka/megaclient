#ifndef MEGAGUARDWIDGET_H
#define MEGAGUARDWIDGET_H

#include "tablelistwidget.h"

class MegaGuardWidget : public QWidget, public Singleton<MegaGuardWidget>
{
    Q_OBJECT
protected:
    MegaGuardWidget();
    friend class Singleton<MegaGuardWidget>;
public:
    void setMode(const Privileges&);
private slots:
    void ok();
    void cancel();
    void hideError();
private:
    void show();
    QLabel *passLabel;
    QLabel *errorLabel;
    QLineEdit *passLineEdit;
    QGroupBox *groupBox;
    QPushButton *okButton;
    QPushButton *cancelButton;
};

#endif // MEGAGUARDWIDGET_H
