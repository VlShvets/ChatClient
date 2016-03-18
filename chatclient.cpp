#include "chatclient.h"

/// Класс ChatWidget

ChatWidget::ChatWidget(QString _userName, QWidget *_parent)
    : QWidget(_parent), chatClient((ChatClient *) _parent)
{
    setObjectName(_userName);

    txtInfo = new QTextEdit;
    txtInput = new QTextEdit;

    txtInfo->setReadOnly(true);

    QPushButton *pcmd = new QPushButton(style()->standardIcon(QStyle::SP_ArrowForward), tr("&Отправить"));
    connect(pcmd, SIGNAL(clicked()), this, SLOT(slotSendToServer()));

    QVBoxLayout *vbxLayout = new QVBoxLayout;
    vbxLayout->addWidget(txtInfo, 10);
    vbxLayout->addWidget(txtInput, 1);
    vbxLayout->addWidget(pcmd, 0, Qt::AlignCenter);
    setLayout(vbxLayout);
}

ChatWidget::~ChatWidget()
{
    delete txtInfo;
    delete txtInput;
}

void ChatWidget::slotSendToServer()
{
    QString message = txtInput->toPlainText();
    if(!message.isEmpty() && !message.isNull())
    {
        txtInfo->append(QTime::currentTime().toString() + " <b>" + chatClient->objectName() +
                        "</b><br>" + message + "<br>");
        chatClient->sendToServer(objectName(), message);
        txtInput->clear();
    }
}

/// Класс ChatClient

ChatClient::ChatClient(QWidget *_parent)
    : QMainWindow(_parent), nextBlockSize(0)
{
    tcpSocket = new QTcpSocket(this);

    QString hostName;
    quint16 nPort;
    InputDialog *inputDialog = new InputDialog;
    if(inputDialog->exec() == QDialog::Accepted)
    {
        hostName = inputDialog->hostName();
        if(hostName.isNull() || hostName.isEmpty())
            hostName = "localhost";
        nPort = inputDialog->port();
        delete inputDialog;
    }
    else
    {
        delete inputDialog;
        exit(0);
    }

    bool bOk;
    QString clientName = QInputDialog::getText(0, tr("Имя пользователя"), tr("Введите имя пользователя:"),
                                        QLineEdit::Normal, "unnamed", &bOk);
    if(!bOk || clientName.isNull() || clientName.isEmpty())
        clientName = "unnamed";
    setWindowTitle(clientName);
    setObjectName(clientName);
    tcpSocket->setObjectName(clientName);

    tcpSocket->connectToHost(hostName, nPort);

    connect(tcpSocket, SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));

    addressee = new QLabel(tr("<b>Пользователи</b>"));

    QToolBar *toolBar = new QToolBar;
    toolBar->setMovable(false);

    toolBar->addAction(style()->standardIcon(QStyle::SP_ArrowBack), tr("Пользователи"), this, SLOT(showListClients()));
    toolBar->addSeparator();
    toolBar->addWidget(addressee);

    addToolBar(toolBar);

    QStackedWidget *stackedWidget = new QStackedWidget;

    QListView *lvClients = new QListView;
    lvClients->setModel(&model);
    lvClients->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(lvClients, SIGNAL(clicked(QModelIndex)), this, SLOT(slotSetCurrentChat(QModelIndex)));
    stackedWidget->addWidget(lvClients);

    setCentralWidget(stackedWidget);
}

ChatClient::~ChatClient()
{
    delete tcpSocket;
    delete addressee;
}

void ChatClient::slotConnected()
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out << quint16(0) << true << tcpSocket->objectName();

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

void ChatClient::slotReadyRead()
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_5_4);
    for(;;)
    {
        if(!nextBlockSize)
        {
            if(tcpSocket->bytesAvailable() < sizeof(quint16))
                break;
            in >> nextBlockSize;
        }

        if(tcpSocket->bytesAvailable() < nextBlockSize)
            break;

        bool initialization;
        in >> initialization;

        if(initialization)
        {
            QString listClients;
            in >> listClients;

            QStringList slClients = listClients.split("0x00", QString::SkipEmptyParts);
            slClients.removeAll(tcpSocket->objectName());
            foreach(QString clientName, slClients)
            {
                if(indexOfChat(clientName) == -1)
                   ((QStackedWidget *) centralWidget())->addWidget(new ChatWidget(clientName, this));
            }

            model.setStringList(slClients);
        }
        else
        {
            QString str;
            in >> str;

            QStringList senderMessage = str.split("0x00", QString::SkipEmptyParts);
            QString sender = senderMessage.first();
            QString message = senderMessage.last();

            ((ChatWidget *) ((QStackedWidget *) centralWidget())->widget(indexOfChat(sender)))->txtInfo->append(QTime::currentTime().toString() +
                                                                                                                " <b>" + sender + "</b><br>" + message + "<br>");
            ((QStackedWidget *) centralWidget())->setCurrentIndex(indexOfChat(sender));
            addressee->setText("<b>" + sender + "</b>");
        }

        nextBlockSize = 0;
    }
}

void ChatClient::slotError(QAbstractSocket::SocketError _err)
{
    QString strError = _err == QAbstractSocket::HostNotFoundError ? tr("Хост не был найден.") :
                       _err == QAbstractSocket::RemoteHostClosedError ? tr("Удаленный хост закрыт.") :
                       _err == QAbstractSocket::ConnectionRefusedError ? tr("В соединении было отказано.") :
                       QString(tcpSocket->errorString());
    QMessageBox::critical(0, tr("Ошибка"), tr("Невозможно установить соединение:") + " " + strError);
    exit(0);
}

void ChatClient::showListClients()
{
    ((QStackedWidget *) centralWidget())->setCurrentIndex(0);
    addressee->setText("<b>" + tr("Пользователи") + "</b>");
}

void ChatClient::slotSetCurrentChat(QModelIndex _modelIndex)
{
    ((QStackedWidget *) centralWidget())->setCurrentIndex(indexOfChat(model.stringList().at(_modelIndex.row())));
    addressee->setText("<b>" + model.stringList().at(_modelIndex.row()) + "</b>");
}

void ChatClient::sendToServer(const QString &_addressee, const QString &_message)
{
    QString str = _addressee + "0x00" + _message;

    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out << quint16(0) << false << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

int ChatClient::indexOfChat(QString _str)
{
    int count = ((QStackedWidget *) centralWidget())->count();
    for(int i = 0; i < count; ++i)
    {
        if(((QStackedWidget *) centralWidget())->widget(i)->objectName() == _str)
            return i;
    }
    return -1;
}
