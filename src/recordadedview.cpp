#include "recordadedview.h"

RecordAdEdView::RecordAdEdView()
{
    okButton = new QPushButton(tr("Ok"));
    okButton->setStatusTip(tr("Finish the operation"));
    okButton->setIcon(QIcon(":/images/ok.png"));
    connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));

    cancelButton = new QPushButton(tr("Cancel"));
    cancelButton->setStatusTip(tr("Cancel the operation"));
    cancelButton->setIcon(QIcon(":/images/cancel.png"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

    backButton = new QPushButton(tr("Back"));
    backButton->setStatusTip(tr("Back to record's list"));
    backButton->setIcon(QIcon(":/images/back.png"));
    connect(backButton, SIGNAL(clicked()), this, SLOT(cancel()));

    titleLabel = new QLabel(tr("Title:"));
    commentLabel = new QLabel(tr("Comment:"));
    readOnlyLabel = new QLabel(tr("Read only:"));
    typeLabel = new QLabel(tr("Type:"));
    valueLabel = new QLabel(tr("Value:"));

    titleLineEdit = new QLineEdit();
    commentTextEdit = new QTextEdit();
    readOnlyCheckBox = new QCheckBox();
    typeComboBox = new QComboBox();
    typeComboBox->addItem("Double", (uint) QVariant::Double);
    typeComboBox->addItem("Int", (uint) QVariant::Int);
    typeComboBox->addItem("String", (uint) QVariant::String);
    valueLineEdit = new QLineEdit();

    QVBoxLayout *titleLayout = new QVBoxLayout;
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(titleLineEdit);

    QVBoxLayout *commentLayout = new QVBoxLayout;
    commentLayout->addWidget(commentLabel);
    commentLayout->addWidget(commentTextEdit);

    QHBoxLayout *readOnlyLayout = new QHBoxLayout;
    readOnlyLayout->addWidget(readOnlyLabel);
    readOnlyLayout->addWidget(readOnlyCheckBox);
    readOnlyLayout->addSpacerItem(new QSpacerItem(40, 20,
                                                  QSizePolicy::Expanding,
                                                  QSizePolicy::Minimum));

    QVBoxLayout *typeLayout = new QVBoxLayout;
    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(typeComboBox);

    QVBoxLayout *valueLayout = new QVBoxLayout;
    valueLayout->addWidget(valueLabel);
    valueLayout->addWidget(valueLineEdit);

    QVBoxLayout *groupBoxLayout = new QVBoxLayout;
    groupBoxLayout->addLayout(titleLayout);
    groupBoxLayout->addLayout(commentLayout);
    groupBoxLayout->addLayout(readOnlyLayout);
    groupBoxLayout->addLayout(typeLayout);
    groupBoxLayout->addLayout(valueLayout);

    groupBox = new QGroupBox(tr("Record"));
    groupBox->setLayout(groupBoxLayout);

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(backButton);
    buttonLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum,
                                                QSizePolicy::Expanding));
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(groupBox);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void RecordAdEdView::cancel()
{
    RecordListWidget::Instance()->show(table);
}

void RecordAdEdView::ok()
{

    RecordListWidget::Instance()->show(table);
}

void RecordAdEdView::show(const QString &title, const QString &comment,
                          const bool &readOnly, const QVariant::Type &type,
                          const QVariant &value, const QString &table,
                          const RecordAdEdView::Mode &mode)
{
    this->oldTitle = title;
    this->table = table;
    this->readOnly = readOnly ? Qt::Checked : Qt::Unchecked;
    this->typeComboBoxIndex = typeComboBox->findData((uint) type);

    titleLineEdit->setText(title);
    commentTextEdit->setText(comment);
    readOnlyCheckBox->setChecked(readOnly);
    typeComboBox->setCurrentIndex(this->typeComboBoxIndex);
    valueLineEdit->setText(value.toString());
    this->mode = mode;
    switch (mode)
    {
        case ViewMode:
            titleLineEdit->setReadOnly(true);
            commentTextEdit->setReadOnly(true);
            connect(readOnlyCheckBox, SIGNAL(stateChanged(int)),
                    this, SLOT(readOnlyCheckBoxStateChanged()));
            connect(typeComboBox, SIGNAL(currentIndexChanged(int)),
                    this, SLOT(readOnlyComboBoxCurrentIndexChanged()));
            valueLineEdit->setReadOnly(true);
            okButton->setVisible(false);
            cancelButton->setVisible(false);
            backButton->setVisible(true);
            break;
        case EditMode:
        case AddMode:
            okButton->setVisible(true);
            cancelButton->setVisible(true);
            backButton->setVisible(false);
            break;
    }

    MainWindow::Instance()->setCentralWidget(this);
    MainWindow::Instance()->setStatusLabelText("");
}

void RecordAdEdView::readOnlyCheckBoxStateChanged()
{
    readOnlyCheckBox->setCheckState(this->readOnly);
}

void RecordAdEdView::readOnlyComboBoxCurrentIndexChanged()
{
    typeComboBox->setCurrentIndex(this->typeComboBoxIndex);
}
