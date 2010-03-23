#include "settingswidget.h"

SettingsWidget::SettingsWidget()
{
    okButton = new QPushButton(tr("Ok"));
    okButton->setStatusTip(tr("Save changes"));
    okButton->setIcon(QIcon(":/images/ok.png"));
    connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));

    cancelButton = new QPushButton(tr("Cancel"));
    cancelButton->setStatusTip(tr("Cancel changes"));
    cancelButton->setIcon(QIcon(":/images/cancel.png"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

    hostLabel = new QLabel(tr("Host:"));
    portLabel = new QLabel(tr("Port:"));
    passLabel = new QLabel(tr("Administartor's password:"));

    hostLineEdit = new QLineEdit();
    QRegExp regexp("([\\w\\d\\-]+(\\.[\\w\\d\\-]+)+)");
    hostLineEdit->setValidator(new QRegExpValidator(regexp, hostLineEdit));

    portLineEdit = new QLineEdit();
    portLineEdit->setValidator(new QIntValidator(0, 65535, portLineEdit));

    passLineEdit = new QLineEdit();
    passLineEdit->setEchoMode(QLineEdit::Password);

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addSpacerItem(new QSpacerItem(220, 20, QSizePolicy::Fixed,
                                                QSizePolicy::Fixed));
    buttonLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum,
                                                QSizePolicy::Expanding));

    QVBoxLayout *groupLayout = new QVBoxLayout;
    groupLayout->addWidget(hostLabel);
    groupLayout->addWidget(hostLineEdit);
    groupLayout->addWidget(portLabel);
    groupLayout->addWidget(portLineEdit);
    groupLayout->addWidget(passLabel);
    groupLayout->addWidget(passLineEdit);
    groupLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum,
                                               QSizePolicy::Expanding));
    groupBox = new QGroupBox(tr("Settings"));
    groupBox->setLayout(groupLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(groupBox);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void SettingsWidget::show()
{
    MegaTcpSocket *tcpSocket = MegaTcpSocket::Instance();
    hostLineEdit->setText(QVariant(tcpSocket->getHost()).toString());
    portLineEdit->setText(QVariant(tcpSocket->getPort()).toString());
    passLineEdit->setText(MegaGuard::Instance()->getPassword());
    MainWindow::Instance()->setCentralWidget(this);
}

void SettingsWidget::ok()
{
    MegaTcpSocket *tcpSocket = MegaTcpSocket::Instance();
    tcpSocket->setHost(hostLineEdit->text());
    tcpSocket->setPort(QVariant(portLineEdit->text()).toUInt());
    MegaGuard::Instance()->setPassword(passLineEdit->text());
    MainWindow::Instance()->writeSettings();
    TableListWidget::Instance()->show();
}

void SettingsWidget::cancel()
{
    TableListWidget::Instance()->show();
}

