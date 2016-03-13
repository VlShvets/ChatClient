#include "inputdialog.h"

InputDialog::InputDialog(QWidget *_parent)
    : QDialog(_parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
    this->setWindowTitle(tr("Хост и порт"));

    txtHostName = new QLineEdit;
    txtPort = new QLineEdit;

    txtHostName->setText("127.0.0.1");
    txtPort->setText("2323");

    QLabel *lblHostName = new QLabel(tr("&Хоста"));
    QLabel *lblPort = new QLabel(tr("&Порт"));

    lblHostName->setBuddy(txtHostName);
    lblPort->setBuddy(txtPort);

    QPushButton *pbOk = new QPushButton(tr("&Соединить"));
    QPushButton *pbCancel = new QPushButton(tr("&Отмена"));

    connect(pbOk, SIGNAL(clicked()), SLOT(accept()));
    connect(pbCancel, SIGNAL(clicked()), SLOT(reject()));

    QGridLayout *gLayout = new QGridLayout;
    gLayout->addWidget(lblHostName, 0, 0);
    gLayout->addWidget(lblPort, 1, 0);
    gLayout->addWidget(txtHostName, 0, 1);
    gLayout->addWidget(txtPort, 1, 1);
    gLayout->addWidget(pbOk, 2, 0);
    gLayout->addWidget(pbCancel, 2, 1);
    setLayout(gLayout);
}

QString InputDialog::hostName() const
{
    return txtHostName->text();
}

quint16 InputDialog::port() const
{
    return txtPort->text().toInt();
}

