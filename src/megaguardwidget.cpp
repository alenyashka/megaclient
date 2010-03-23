#include "megaguardwidget.h"

MegaGuardWidget::MegaGuardWidget()
{
    okButton = new QPushButton(tr("Ok"));
    okButton->setStatusTip(tr("Enter as administrator"));
    okButton->setIcon(QIcon(":/images/ok.png"));
    connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));

    cancelButton = new QPushButton(tr("Cancel"));
    cancelButton->setStatusTip(tr("Cancel operation"));
    cancelButton->setIcon(QIcon(":/images/cancel.png"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

    passLabel = new QLabel(tr("Enter password:"));
    errorLabel = new QLabel(QString("<font color=red>").append(
            tr("Incorrect password")).append("</font>"));
    errorLabel->setVisible(false);
    passLineEdit = new QLineEdit();
    passLineEdit->setEchoMode(QLineEdit::Password);
    connect(passLineEdit, SIGNAL(textEdited(QString)), this, SLOT(hideError()));

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addSpacerItem(new QSpacerItem(220, 20, QSizePolicy::Fixed,
                                                QSizePolicy::Fixed));
    buttonLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum,
                                                QSizePolicy::Expanding));

    QVBoxLayout *groupLayout = new QVBoxLayout;
    groupLayout->addWidget(passLabel);
    groupLayout->addWidget(passLineEdit);
    groupLayout->addWidget(errorLabel);
    groupLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum,
                                               QSizePolicy::Expanding));

    groupBox = new QGroupBox(tr("Check passowrd"));
    groupBox->setLayout(groupLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(groupBox);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void MegaGuardWidget::ok()
{
    if (passLineEdit->text() == MegaGuard::Instance()->getPassword())
    {
        MegaGuard::Instance()->setMode(Admin);
        TableListWidget::Instance()->show();
    }
    else
    {
        errorLabel->setVisible(true);
    }
}

void MegaGuardWidget::cancel()
{
    TableListWidget::Instance()->show();
}

void MegaGuardWidget::show()
{
    MainWindow::Instance()->setCentralWidget(this);
}

void MegaGuardWidget::hideError()
{
    errorLabel->setVisible(false);
}

void MegaGuardWidget::setMode(const Privileges &m)
{
    switch (m)
    {
        case User:
            MegaGuard::Instance()->setMode(User);
            cancel();
            break;
        case Admin:
            this->show();
            break;
    }
}
