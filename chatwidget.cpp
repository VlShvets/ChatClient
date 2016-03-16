#include "chatwidget.h"

ChatWidget::ChatWidget(QWidget *parent) : QWidget(parent)
{
    txtInfo = new QTextEdit;
    txtInput = new QTextEdit;

    txtInfo->setReadOnly(true);

    QPushButton *pcmd = new QPushButton(tr("&Отправить"));
    pcmd->setMaximumWidth(100);
    connect(pcmd, SIGNAL(clicked()), SLOT(slotSendToServer()));

    QVBoxLayout *vbxLayout = new QVBoxLayout;
    vbxLayout->addWidget(new QLabel("<H1>Чат</H1>"), 0, Qt::AlignCenter);
    vbxLayout->addWidget(txtInfo, 10);
    vbxLayout->addWidget(txtInput, 1);
    vbxLayout->addWidget(pcmd, 0, Qt::AlignCenter);
    this->setLayout(vbxLayout);
}

ChatWidget::~ChatWidget()
{
    delete txtInfo;
    delete txtInput;
}

