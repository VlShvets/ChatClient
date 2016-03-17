#include "chatclient.h"

/// Класс ChatWidget

ChatWidget::ChatWidget(QString _userName, QWidget *_parent)
    : QWidget(_parent)
{
    this->setObjectName(_userName);

    txtInfo = new QTextEdit;
    txtInput = new QTextEdit;

    txtInfo->setReadOnly(true);

    QPushButton *pcmd = new QPushButton(tr("&Отправить"));
    pcmd->setMaximumWidth(100);
    connect(pcmd, SIGNAL(clicked()), this, SLOT(slotSendToServer()));

    QVBoxLayout *vbxLayout = new QVBoxLayout;
    vbxLayout->addWidget(new QLabel(_userName), 0, Qt::AlignCenter);
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

/// Класс ChatClient

ChatClient::ChatClient(QWidget *_parent)
    : QMainWindow(_parent), nextBlockSize(0)
{
    tcpSocket = new QTcpSocket(this);

    QString hostName = "localhost";
    quint16 nPort = 2323;
    InputDialog *inputDialog = new InputDialog;
    if(inputDialog->exec() == QDialog::Accepted)
    {
        hostName = inputDialog->hostName();
        nPort = inputDialog->port();
    }
    delete inputDialog;

    bool bOk;
    QString clientName = QInputDialog::getText(0, tr("Имя пользователя"), tr("Введите имя пользователя:"),
                                        QLineEdit::Normal, "unnamed", &bOk);
    if(!bOk || clientName.isEmpty())
        clientName = "unnamed";
    this->setWindowTitle(clientName);
    tcpSocket->setObjectName(clientName);

    tcpSocket->connectToHost(hostName, nPort);

    connect(tcpSocket, SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));

    QListView *lvClients = new QListView;
    lvClients->setModel(&model);
    lvClients->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(lvClients, SIGNAL(clicked(QModelIndex)), this, SLOT(slotSetCurrentChat(QModelIndex)));
    this->setCentralWidget(lvClients);

    stackedWidget = new QStackedWidget;

    QDockWidget *dockWidget = new QDockWidget;
    dockWidget->setWindowTitle("Чат");
    dockWidget->setWidget(stackedWidget);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    connect(lvClients, SIGNAL(clicked(QModelIndex)), dockWidget, SLOT(show()));
    this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
}

ChatClient::~ChatClient()
{
    delete stackedWidget;
    delete tcpSocket;
}

void ChatClient::slotConnected()
{
    sendToServer(tcpSocket->objectName(), "");
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

        QString sender;
        QTime time;
        QString str;
        in >> sender >> time >> str;

        if(tcpSocket->objectName() == sender)
        {
            QStringList slClients = str.split("0x00", QString::SkipEmptyParts);
            slClients.removeAll(tcpSocket->objectName());

            foreach(QString userName, slClients)
            {
                if(indexOfChat(userName) == -1)
                    stackedWidget->addWidget(new ChatWidget(tcpSocket, userName, this));
            }

            model.setStringList(slClients);
        }
        else
        {
            ((ChatWidget *) stackedWidget->widget(indexOfChat(sender)))->append(time.toString() + " " + str);
        }

        nextBlockSize = 0;
    }
}

void ChatClient::slotSendToServer()
{
    sendToServer(((QPushButton *) sender())->objectName(), ;
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

void ChatClient::slotSetCurrentChat(QModelIndex _modelIndex)
{
    stackedWidget->setCurrentIndex(indexOfChat(model.stringList().at(_modelIndex.row())));
}

void ChatClient::sendToServer(const QString &_addressee, const QString &_message)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out << quint16(0) << _addressee <<  QTime::currentTime() << _message;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

int ChatClient::indexOfChat(QString _str)
{
    for(int i = 0; i < stackedWidget->count(); ++i)
    {
        if(stackedWidget->widget(i)->objectName() == _str)
            return i;
    }
    return -1;
}
